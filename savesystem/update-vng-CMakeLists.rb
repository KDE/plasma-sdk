#!/usr/bin/env ruby

if $0 != "./update-vng-CMakeLists.rb" then
	puts "FATAL ERROR"
	puts "you have to run this script from the directory it's in"
	exit
end

puts "The text to paste in CMakeLists.txt is:"
puts "---------------------------------------"
puts
puts "set(vng_SRCS"

Dir['vng/*'].each do |file|
	puts file if file.end_with? '.cpp'
end

Dir['vng/*/*'].each do |file|
	puts file if file.end_with? '.cpp'
end

puts ')'





