" ==============================================================================
"        File: vng.vim
"      Author: Srinath Avadhanula (srinathava [AT] gmail [DOT] com)
"        Help: vng.vim is a plugin which implements some simple mappings
"              to make it easier to use vng, David Roundy's distributed
"              revisioning system.
"   Copyright: Srinath Avadhanula
"     License: Vim Charityware license. (:help uganda)
"
" As of right now, this script provides the following mappings:
"
" 	<Plug>VngVertDiffsplit
" 		This mapping vertically diffsplit's the current window with the
" 		last recorded version.
"
" 		Default map: <leader>dkv
"
" 	<Plug>VngIneractiveDiff
" 		This mapping displays the changes for the current file. The user
" 		can then choose two revisions from the list using the 'f' (from)
" 		and 't' (to) keys. After choosing two revisions, the user can press
" 		'g' at which point, the delta between those two revisions is
" 		displayed in two veritcally diffsplit'ted windows.
"
" 		Default map: <leader>dki
"
" 	<Plug>VngStartCommitSession
" 		This mapping starts an interactive commit console using which the
" 		user can choose which hunks to include in the record and then also
" 		write out a patch name and commit message.
"
" 		Default map: <leader>dkc
" ==============================================================================

" ==============================================================================
" Main functions
" ==============================================================================
" Functions related to diffsplitting current file with last recorded {{{

nmap <Plug>VngVertDiffsplit :call Vng_DiffSplit()<CR>
if !hasmapto('<Plug>VngVertDiffsplit')
	nmap <unique> <leader>dkv <Plug>VngVertDiffsplit
endif

" Vng_DiffSplit: diffsplits the current file with last recorded version {{{
function! Vng_DiffSplit()
	let origDir = getcwd()
	call Vng_CD(expand('%:p:h'))

	" first find out the location of the repository by searching upwards
	" from the current directory for a _vng* directory.
	if Vng_SetVngDirectory() < 0
        return -1
    endif

	" Find out if the current file has any changes.
	if Vng_System('vng whatsnew '.expand('%')) =~ '\(^\|\n\)No changes'
		echomsg "vng says there are no changes"
		return 0
	endif

    " Find out the relative path name from the parent dir of the _vng
    " directory to the location of the presently edited file
    let relPath = strpart(expand('%:p'), strlen(b:vng_repoDirectory))

    " The last recorded file lies in
    "   $DIR/_vng/current/relative/path/to/file
    " where
    "   $DIR/relative/path/to/file
    " is the actual path of the file
    let lastRecFile = b:vng_repoDirectory . '/'
        \ . '_vng/current' . relPath

	call Vng_Debug(':Vng_DiffSplit: lastRecFile = '.lastRecFile)

	call Vng_NoteViewState()

	execute 'vert diffsplit '.lastRecFile
	" The file in _vng/current should not be edited by the user.
	setlocal readonly

	nmap <buffer> q <Plug>VngRestoreViewAndQuit

	call Vng_CD(origDir)
endfunction

" }}}
" Vng_SetVngDirectory: finds which _vng directory contains this file {{{
" Description: This function searches upwards from the current directory for a
"              _vng directory.
function! Vng_SetVngDirectory()
	let filename = expand('%:p')
	let origDir = getcwd()
	let foundFile = 0

	call Vng_CD(expand('%:p:h'))
	let lastDir = getcwd()
    while glob('_vng*') == ''
        lcd ..
        " If we cannot go up any further, then break
        if lastDir == getcwd()
            break
        endif
		let lastDir = getcwd()
    endwhile

    " If a _vng directory was never found, then quit...
    if glob('_vng*') == ''
        echohl Error
        echo "_vng directory not found in or above current directory"
        echohl None
        return -1
    endif

	let b:vng_repoDirectory = getcwd()
	
	call Vng_CD(origDir)

    return 0
endfunction

" }}}

" }}}
" Functions related to interactive diff splitting {{{

nmap <Plug>VngIneractiveDiff :call Vng_StartInteractiveDiffSplit()<CR>
if !hasmapto('<Plug>VngIneractiveDiff')
	nmap <unique> <leader>dki <Plug>VngIneractiveDiff
endif

" Vng_StartInteractiveDiffSplit: initializes the interactive diffsplit session {{{
function! Vng_StartInteractiveDiffSplit()
	let origdir = getcwd()
	let filedir = expand('%:p:h')
	let filename = expand('%:p:t')
	let filetype = &l:ft

	call Vng_CD(filedir)
	call Vng_OpenScratchBuffer()

	let changelog = Vng_FormatChangelog(filename)
	if changelog == ''
		echomsg "vng does not know about this file"
		return
	endif
	0put=Vng_FormatChangelog(filename)

	call Vng_CD(origdir)

	set ft=changelog
	" reading in stuff creates an extra line ending
	$ d _

	let header = 
		\ '====[ Vng diff console: Mappings ]==========================' . "\n" .
		\ "\n" .
		\ 'j/k	: next/previous patch' . "\n" .
		\ 'f  	: set "from" patch' . "\n" .
		\ 't  	: set "to" patch' . "\n" .
		\ 'g	: proceed with diff (Go)' . "\n" .
		\ 'q	: quit without doing anything further'. "\n" .
		\ '=============================================================='
	0put=header
	normal! gg
	
	let b:vng_FROM_hash = ''
	let b:vng_TO_hash = ''
	let b:vng_orig_ft = filetype
	let b:vng_orig_file = filename
	let b:vng_orig_dir = filedir

	nmap <buffer> j <Plug>VngGotoNextChange
	nmap <buffer> k <Plug>VngGotoPreviousChange
	nmap <buffer> f <Plug>VngSetFromHash
	nmap <buffer> t <Plug>VngSetToHash
	nmap <buffer> g <Plug>VngProceedWithDiffSplit
	nmap <buffer> q :q<CR>:<BS>

	call search('^\s*\*')
endfunction

" }}}
" Vng_FormatChangelog: creates a Changelog with hash for a file {{{
function! Vng_FormatChangelog(filename)

	" remove the first <created_as ../> entry
	let xmloutput = Vng_System('vng changes --xml-output '.a:filename)
	" TODO: We need to see if there were any errors generated by vng or
	" if the xmloutput is empty.
	
	let idx = match(xmloutput, '<\/created_as>')
	let xmloutput = strpart(xmloutput, idx + strlen('</created_as>'))

	let changelog = ""

	" For each patch in the xml output
	let idx = match(xmloutput, '</patch>')
	while idx != -1
		" extract the actual patch.
		let patch = strpart(xmloutput, 0, idx)

		" From the patch, extract the various fields.
		let name = matchstr(patch, '<name>\zs.*\ze<\/name>')
		let comment = matchstr(patch, '<comment>\zs.*\ze</comment>')

		let pattern = ".*<patch author='\\(.\\{-}\\)' date='\\(.\\{-}\\)' local_date='\\(.\\{-}\\)' inverted='\\(True\\|False\\)' hash='\\(.\\{-}\\)'>.*"

		let author = substitute(patch, pattern, '\1', '')
		let date = substitute(patch, pattern, '\2', '')
		let local_date = substitute(patch, pattern, '\3', '')
		let inverted = substitute(patch, pattern, '\4', '')
		let hash = substitute(patch, pattern, '\5', '')

		" Create the formatted changelog entry for this patch.
		" Unfortunately, we do not have the +M -5 +10 kind of line in this
		" output. Price to pay...
		let formatted = "[hash ".hash."]\n".local_date.' '.author."\n  * ".name."\n".
					\ substitute(comment, "\n", "\n  ", 'g')."\n\n"

		let changelog = changelog.formatted

		" Once the patch has been processed, snip if from the xml output.
		let xmloutput = strpart(xmloutput, idx + strlen('</patch>'))
		let idx = match(xmloutput, '</patch>')
	endwhile
	
	if changelog != ''
		let changelog = "Changes in ".a:filename.":\n\n".changelog

		let s:sub_{'&apos;'} = "'"
		let s:sub_{'&quot;'} = '"'
		let s:sub_{'&amp;'} = '&'
		let s:sub_{'&lt;'} = '<'
		let s:sub_{'&gt;'} = '>'

		function! Eval(expr)
			if exists("s:sub_{'".a:expr."'}")
				return s:sub_{a:expr}
			else
				return a:expr
			endif
		endfunction

		let changelog = substitute(changelog, '&\w\{2,4};', '\=Eval(submatch(0))', 'g')

		return changelog

	else
		return ""
	endif

endfunction

" }}}
" Vng_SetHash: remembers the user's from/to hashs for the diff {{{

nnoremap <Plug>VngSetToHash :call Vng_SetHash('TO')<CR>
nnoremap <Plug>VngSetFromHash :call Vng_SetHash('FROM')<CR>

function! Vng_SetHash(fromto)
	normal! $
	call search('^\s*\*', 'bw')

	" First remove any previous mark set.
	execute '% s/'.a:fromto.'$//e'

	" remember the present line's hash
	let b:vng_{a:fromto}_hash = matchstr(getline(line('.')-2), '\[hash \zs.\{-}\ze\]')

	call setline(line('.')-1, getline(line('.')-1).'  '.a:fromto)
endfunction

" }}}
" Vng_GotoChange: goes to the next previous change {{{

nnoremap <Plug>VngGotoPreviousChange :call Vng_GotoChange('b')<CR>
nnoremap <Plug>VngGotoNextChange :call Vng_GotoChange('')<CR>

function! Vng_GotoChange(dirn)
	call search('^\s*\*', a:dirn.'w')
endfunction

" }}}
" Vng_ProceedWithDiff: proceeds with the actual diff between requested versions {{{
" Description: 

nnoremap <Plug>VngProceedWithDiffSplit :call Vng_ProceedWithDiff()<CR>

function! Vng_ProceedWithDiff()
	call Vng_Debug('+Vng_ProceedWithDiff')

	if b:vng_FROM_hash == '' && b:vng_TO_hash == ''
		echohl Error
		echo "You need to set at least one of the FROM or TO hashes"
		echohl None
		return -1
	endif

	let origDir = getcwd()

	call Vng_CD(b:vng_orig_dir)

	let ft = b:vng_orig_ft
	let filename = b:vng_orig_file
	let fromhash = b:vng_FROM_hash
	let tohash = b:vng_TO_hash

	" quit the window which shows the Changelog
	q

	" First copy the present file into a temporary location
	let tmpfile = tempname().'__present'

	call Vng_System('cp '.filename.' '.tmpfile)

	if fromhash != ''
		let tmpfilefrom = tempname().'__from'
		call Vng_RevertToState(filename, fromhash, tmpfilefrom)
		let file1 = tmpfilefrom
	else
		let file1 = filename
	endif

	if tohash != ''
		let tmpfileto = tempname().'__to'
		call Vng_RevertToState(filename, tohash, tmpfileto)
		let file2 = tmpfileto
	else
		let file2 = filename
	endif

	call Vng_Debug(':Vng_ProceedWithDiff: file1 = '.file1.', file2 = '.file2)

	execute 'split '.file1
	execute "nnoremap <buffer> q :q\<CR>:e ".file2."\<CR>:q\<CR>"
	let &l:ft = ft
	execute 'vert diffsplit '.file2
	let &l:ft = ft
	execute "nnoremap <buffer> q :q\<CR>:e ".file1."\<CR>:q\<CR>"
	
	call Vng_CD(origDir)

	call Vng_Debug('-Vng_ProceedWithDiff')
endfunction 

" }}}
" Vng_RevertToState: reverts a file to a previous state {{{
function! Vng_RevertToState(filename, patchhash, tmpfile)
	call Vng_Debug('+Vng_RevertToState: pwd = '.getcwd())

	let syscmd = "vng diff --from-match \"hash ".a:patchhash."\" ".a:filename.
		\ " | patch -R ".a:filename." -o ".a:tmpfile
	call Vng_Debug(':Vng_RevertToState: syscmd = '.syscmd)
	call Vng_System(syscmd)

	let syscmd = "vng diff --match \"hash ".a:patchhash."\" ".a:filename.
		\ " | patch ".a:tmpfile
	call Vng_Debug(':Vng_RevertToState: syscmd = '.syscmd)
	call Vng_System(syscmd)

	call Vng_Debug('-Vng_RevertToState')
endfunction 

" }}}

" }}}
" Functions related to interactive comitting {{{

nmap <Plug>VngStartCommitSession :call Vng_StartCommitSession()<CR>
if !hasmapto('<Plug>VngStartCommitSession')
	nmap <unique> <leader>dkc <Plug>VngStartCommitSession
endif

" Vng_StartCommitSession: start an interactive commit "console" {{{
function! Vng_StartCommitSession()

	let origdir = getcwd()
	let filename = expand('%:p:t')
	let filedir = expand('%:p:h')
	
	call Vng_CD(filedir)
	let wn = Vng_System('vng whatsnew --no-summary --dont-look-for-adds')
	call Vng_CD(origdir)

	if wn =~ '^No changes!'
		echo "No changes seen by vng"
		return
	endif

	" read in the contents of the `vng whatsnew` command
	call Vng_SetSilent('silent')

	" opens a scratch buffer for the commit console
	" Unfortunately, it looks like the temporary file has to exist in the
	" present directory because at least on Windows, vng doesn't want to
	" handle absolute path names containing ':' correctly.
	exec "top split ".Vng_GetTempName(expand('%:p:h'))
	0put=wn

	" Delete the end and beginning markers
	g/^[{}]$/d _
	" Put an additional four spaces in front of all lines and a little
	" checkbox in front of the hunk specifier lines
	% s/^/    /
	% s/^    hunk/[ ] hunk/

	let header = 
		\ '====[ Vng commit console: Mappings ]========================' . "\n" .
		\ "\n" .
		\ 'J/K	: next/previous hunk' . "\n" .
		\ 'Y/N	: accept/reject this hunk' . "\n" .
		\ 'F/S	: accept/reject all hunks from this file' . "\n" .
		\ 'A/U	: accept/reject all (remaining) hunks' . "\n" .
		\ 'q	: quit this session without committing' . "\n" .
		\ 'L	: goto log area to record description'. "\n" .
		\ 'R	: done! finish record' . "\n" .
		\ '=============================================================='
	0put=header

	let footer = 
		\ '====[ Vng commit console: Commit log description ]==========' . "\n" .
		\ "\n" .
		\ '***VNG***'. "\n".
		\ 'Write the long patch description in this area.'. "\n".
		\ 'The first line in this area will be the patch name.'. "\n".
		\ 'Everything in this area from the above ***VNG*** line on '."\n".
		\ 'will be ignored.'. "\n" .
		\ '=============================================================='
	$put=footer

	set nomodified
	call Vng_SetSilent('notsilent')

	" Set the fold expression so that things get folded up nicely.
	set fdm=expr
	set foldexpr=Vng_WhatsNewFoldExpr(v:lnum)

	" Finally goto the first hunk
	normal! G
	call search('^\[ ', 'w')

	nnoremap <buffer> J :call Vng_GotoHunk('')<CR>:<BS>
	nnoremap <buffer> K :call Vng_GotoHunk('b')<CR>:<BS>

	nnoremap <buffer> Y :call Vng_SetHunkVal('y')<CR>:<BS>
	nnoremap <buffer> N :call Vng_SetHunkVal('n')<CR>:<BS>

	nnoremap <buffer> A :call Vng_SetRemainingHunkVals('y')<CR>:<BS>
	nnoremap <buffer> U :call Vng_SetRemainingHunkVals('n')<CR>:<BS>

	nnoremap <buffer> F :call Vng_SetFileHunkVals('y')<CR>:<BS>
	nnoremap <buffer> S :call Vng_SetFileHunkVals('n')<CR>:<BS>

	nnoremap <buffer> L :call Vng_GotoLogArea()<CR>
	nnoremap <buffer> R :call Vng_FinishCommitSession()<CR>

	nnoremap <buffer> q :call Vng_DeleteTempFile(expand('%:p'))<CR>

	let b:vng_orig_file = filename
	let b:vng_orig_dir = filedir
endfunction

" }}}
" Vng_WhatsNewFoldExpr: foldexpr function for a commit console {{{
function! Vng_WhatsNewFoldExpr(lnum)
	if matchstr(getline(a:lnum), '^\[[yn ]\]') != ''
		return '>1'
	elseif matchstr(getline(a:lnum+1), '^====\[ .* log description') != ''
		return '<1'
	else
		return '='
	endif
endfunction

" }}}
" Vng_GotoHunk: goto next/previous hunk in a commit console {{{
function! Vng_GotoHunk(dirn)
	call search('^\[[yn ]\]', a:dirn.'w')
endfunction

" }}}
" Vng_SetHunkVal: accept/reject a hunk for committing {{{
function! Vng_SetHunkVal(yesno)
	if matchstr(getline('.'), '\[[yn ]\]') == ''
		return
	end
	execute "s/^\\[.\\]/[".a:yesno."]"
	call Vng_GotoHunk('')
endfunction

" }}}
" Vng_SetRemainingHunkVals: accept/reject all remaining hunks {{{
function! Vng_SetRemainingHunkVals(yesno)
	execute "% s/^\\[ \\]/[".a:yesno."]/e"
endfunction 

" }}}
" Vng_SetFileHunkVals: accept/reject all hunks from this file {{{
function! Vng_SetFileHunkVals(yesno)
	" If we are not on a hunk line for some reason, then do not do
	" anything.
	if matchstr(getline('.'), '\[[yn ]\]') == ''
		return
	end

	" extract the file name from the current line
	let filename = matchstr(getline('.'), 
		\ '^\[[yn ]\] hunk \zs\f\+\ze')
	if filename == ''
		return
	endif

	" mark all hunks belonging to the file with yes/no
	execute '% s/^\[\zs[yn ]\ze\] hunk '.escape(filename, "\\/").'/'.a:yesno.'/e'

	call Vng_GotoHunk('')
endfunction 

" }}}
" Vng_GotoLogArea: records the log description of the commit {{{
function! Vng_GotoLogArea()
	if search('^\[ \]')
		echohl WarningMsg
		echo "There are still some hunks which are neither accepted or rejected"
		echo "Please set the status of all hunks before proceeding to log"
		echohl None

		return
	endif

	call search('\M^***VNG***')
	normal! k
	startinsert
endfunction 

" }}}
" Vng_FinishCommitSession: finishes the interactive commit session {{{
function! Vng_FinishCommitSession()

	" First make sure that all hunks have been set as accpeted or rejected.
	if search('^\[ \]')

		echohl WarningMsg
		echo "There are still some hunks which are neither accepted or rejected"
		echo "Please set the status of all hunks before proceeding to log"
		echohl None

		return
	endif

	" Then make a list of the form "ynyy..." from the choices made by the
	" user.
	let yesnolist = ''
	g/^\[[yn]\]/let yesnolist = yesnolist.matchstr(getline('.'), '^\[\zs.\ze\]')

	" make sure that a valid log message has been written.
	call search('====\[ Vng commit console: Commit log description \]', 'w')
	normal! j
	let _k = @k
	execute "normal! V/\\M***VNG***/s\<CR>k\"ky"

	let logMessage = @k
	let @k = _k

	call Vng_Debug(':Vng_FinishCommitSession: logMessage = '.logMessage)
	call Vng_Debug(':Vng_FinishCommitSession: yesnolist = ['.yesnolist.']')

	if logMessage !~ '[[:graph:]]'

		echohl WarningMsg
		echo "The log message is either ill formed or empty"
		echo "Please repair the mistake and retry"
		echohl None

		return
	endif

	" Remove everything from the file except the log file.
	% d _
	0put=logMessage
	$ d _
	w

	let origDir = getcwd()
	call Vng_CD(b:vng_orig_dir)

	let vngOut = Vng_System('echo '.yesnolist.' | vng record --logfile='.expand('%:p:t'))

	call Vng_CD(origDir)

	% d _
	0put=vngOut

	let footer = 
		\ "=================================================================\n".
		\ "Press q to delete this temporary file and quit the commit session\n".
		\ "If you quit this using :q, then a temp file will remain in the \n".
		\ "present directory\n".
		\ "================================================================="
	$put=footer

	set nomodified

endfunction 

" }}}
" Vng_DeleteTempFile: deletes temp file created during commit session {{{
function! Vng_DeleteTempFile(fname)
	call Vng_Debug('+Vng_DeleteTempFile: fname = '.a:fname.', bufnr = '.bufnr(a:fname))
	if bufnr(a:fname) > 0
		execute 'bdelete! '.bufnr(a:fname)
	endif
	let sysout = Vng_System('rm '.a:fname)
endfunction 

" }}}

" }}}

" ==============================================================================
" Helper functions
" ==============================================================================
" Vng_System: runs a system command escaping quotes {{{

" By default we will use python if available.
if !exists('g:Vng_UsePython')
	let g:Vng_UsePython = 1
endif

function! Vng_System(syscmd)
	if has('python') && g:Vng_UsePython == 1
		execute 'python pySystem(r"""'.a:syscmd.'""")'
		return retval
	else
		if &shellxquote =~ '"'
			return system(escape(a:syscmd, '"'))
		else
			return system(a:syscmd)
		endif
	endif
endfunction 

if has('python') && g:Vng_UsePython == 1
python << EOF

import vim, os, re

def pySystem(command):
	(cstdin, cstdout) = os.popen2(command)

	out = cstdout.read()

	vim.command("""let retval = "%s" """ % re.sub(r'"|\\', r'\\\g<0>', out))

EOF
endif

" }}}
" Vng_CD: cd's to a directory {{{
function! Vng_CD(dirname)
	execute "cd ".escape(a:dirname, ' ')
endfunction

" }}}
" Vng_OpenScratchBuffer: opens a scratch buffer {{{
function! Vng_OpenScratchBuffer()
	new
	set buftype=nofile
	set noswapfile
	set filetype=
endfunction

" }}}
" Vng_NoteViewState: notes the current fold related settings of the buffer {{{
function! Vng_NoteViewState()
	let b:vng_old_diff         = &l:diff 
	let b:vng_old_foldcolumn   = &l:foldcolumn
	let b:vng_old_foldenable   = &l:foldenable
	let b:vng_old_foldmethod   = &l:foldmethod
	let b:vng_old_scrollbind   = &l:scrollbind
	let b:vng_old_wrap         = &l:wrap
endfunction

" }}}
" Vng_ResetViewState: restores the fold related settings of a buffer {{{
function! Vng_ResetViewState()
	let &l:diff         = b:vng_old_diff 
	let &l:foldcolumn   = b:vng_old_foldcolumn
	let &l:foldenable   = b:vng_old_foldenable
	let &l:foldmethod   = b:vng_old_foldmethod
	let &l:scrollbind   = b:vng_old_scrollbind
	let &l:wrap         = b:vng_old_wrap
endfunction

nnoremap <Plug>VngRestoreViewAndQuit :q<CR>:call Vng_ResetViewState()<CR>:<BS> 

" }}}
" Vng_GetTempName: get the name of a temporary file in specified directory {{{
" Description: Unlike vim's native tempname(), this function returns the name
"              of a temporary file in the directory specified. This enables
"              us to create temporary files in a specified directory.
function! Vng_GetTempName(dirname)
	let prefix = 'vngVimTemp'
	let slash = (a:dirname =~ '\\\|/$' ? '' : '/')
	let i = 0
	while filereadable(a:dirname.slash.prefix.i.'.tmp') && i < 1000
		let i = i + 1
	endwhile
	if filereadable(a:dirname.slash.prefix.i.'.tmp')
		echoerr "Temporary file could not be created in ".a:dirname
		return ''
	endif
	return expand(a:dirname.slash.prefix.i.'.tmp', ':p')
endfunction
" }}}
" Vng_SetSilent: sets options to make vim "silent" {{{
function! Vng_SetSilent(isSilent)
	if a:isSilent == 'silent'
		let s:_showcmd = &showcmd
		let s:_lz = &lz
		let s:_report = &report
		set noshowcmd
		set lz
		set report=999999
	else
		let &showcmd = s:_showcmd
		let &lz = s:_lz
		let &report = s:_report
	endif
endfunction " }}}
" Vng_Debug: appends the argument into s:debugString {{{
if !exists('g:Vng_Debug')
	let g:Vng_Debug = 1
	let s:debugString = ''
endif
function! Vng_Debug(str)
	let s:debugString = s:debugString.a:str."\n"
endfunction " }}}
" Vng_PrintDebug: prings s:debugString {{{
function! Vng_PrintDebug()
	echo s:debugString
endfunction " }}}
" Vng_ClearDebug: clears the s:debugString string {{{
function! Vng_ClearDebug(...)
	let s:debugString = ''
endfunction " }}}

" vim:fdm=marker
