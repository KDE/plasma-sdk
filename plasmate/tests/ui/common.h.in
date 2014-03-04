#include <QApplication>
#include <QStandardPaths>

#define PLASMATE_TEST_MAIN(TestObject) \
int main(int argc, char **argv) \
{ \
    QString appName = #TestObject + QStringLiteral("test"); \
    QApplication::setApplicationName(appName); \
    QApplication app(argc, argv); \
    QStandardPaths::setTestModeEnabled(true); \
    TestObject *obj = new TestObject(); \
    obj->show(); \
    return app.exec(); \
}

