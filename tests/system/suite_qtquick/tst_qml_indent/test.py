source("../../shared/qtcreator.py")

def main():
    global workingDir
    startApplication("qtcreator" + SettingsPath)
    # using a temporary directory won't mess up an eventually exisiting
    workingDir = tempDir()
    createNewQtQuickApplication(workingDir, "untitled")
    # wait for parsing to complete
    waitForSignal("{type='CppTools::Internal::CppModelManager' unnamed='1'}", "sourceFilesRefreshed(QStringList)", 5000)
    if not prepareQmlFile():
        invokeMenuItem("File", "Save All")
        invokeMenuItem("File", "Exit")
        return
    testReIndent()
    invokeMenuItem("File", "Save All")
    invokeMenuItem("File", "Exit")

def prepareQmlFile():
    # make sure the QML file is opened
    navTree = waitForObject("{type='Utils::NavigationTreeView' unnamed='1' visible='1' "
                            "window=':Qt Creator_Core::Internal::MainWindow'}", 20000)
    model = navTree.model()
    waitForObjectItem(navTree, "untitled.QML.qml/untitled.main\\.qml")
    doubleClickItem(navTree, "untitled.QML.qml/untitled.main\\.qml", 5, 5, 0, Qt.LeftButton)
    editor = waitForObject("{type='QmlJSEditor::QmlJSTextEditorWidget' unnamed='1' visible='1' "
                           "window=':Qt Creator_Core::Internal::MainWindow'}", 20000)
    for i in range(3):
        content = "%s" % editor.plainText
        start = content.find("Text {")
        end = content.rfind("}")
        end = content.rfind("}", end-1)
        if start==-1 or end==-1:
            test.fatal("Couldn't find line(s) I'm looking for - QML file seems to "
                       "have changed!\nLeaving test...")
            return False
        markText(editor, start, end)
        type(editor, "<Ctrl+C>")
        for i in range(10):
            type(editor, "<Ctrl+V>")
    global originalText
    # assume the current editor content to be indented correctly
    originalText = "%s" % editor.plainText
    indented = editor.plainText
    unindented = ""
    lines = str(indented).split("\n")
    test.log("Using %d lines..." % len(lines))
    for line in lines:
        unindented += line.lstrip()+"\n"
    unindented=unindented[0:-1]
    editor.plainText = unindented
    return True

def handleTextChanged(object):
    global textHasChanged
    textHasChanged = True

def testReIndent():
    global originalText,textHasChanged
    installLazySignalHandler("QmlJSEditor::QmlJSTextEditorWidget", "textChanged()", "handleTextChanged")
    textHasChanged = False
    editor = waitForObject("{type='QmlJSEditor::QmlJSTextEditorWidget' unnamed='1' visible='1' "
                           "window=':Qt Creator_Core::Internal::MainWindow'}")
    type(editor, "<Ctrl+A>")
    test.log("calling re-indent")
    type(editor, "<Ctrl+I>")
    waitFor("textHasChanged==True", 20000)
    textAfterReIndent = "%s" % editor.plainText
    if originalText==textAfterReIndent:
        test.passes("Text successfully reindented...")
    else:
        # shrink the texts - it's huge output that takes long time to finish & screenshot is taken as well
        originalText = shrinkText(originalText, 20)
        textAfterReIndent = shrinkText(textAfterReIndent, 20)
        test.fail("Re-indent of text unsuccessful...",
                  "Original (first 20 lines):\n%s\n\n______________________\nAfter re-indent (first 20 lines):\n%s"
                  % (originalText, textAfterReIndent))

def shrinkText(txt, lines=10):
    count = 0
    index = -1
    while count<lines:
        index=originalText.find("\n", index+1)
        if index==-1:
            break
        count += 1
    return originalText[0:index]

def cleanup():
    global workingDir
    # waiting for a clean exit - for a full-remove of the temp directory
    waitForCleanShutdown()
    if workingDir!=None:
        deleteDirIfExists(workingDir)

