#ifndef QMLJS_QMLJSUTILS_H
#define QMLJS_QMLJSUTILS_H

#include "qmljs_global.h"
#include "parser/qmljsastfwd_p.h"
#include "parser/qmljsengine_p.h"

#include <QtGui/QColor>

namespace QmlJS {

QMLJS_EXPORT QColor toQColor(const QString &qmlColorString);
QMLJS_EXPORT QString toString(AST::UiQualifiedId *qualifiedId,
                              const QChar delimiter = QLatin1Char('.'));

QMLJS_EXPORT AST::SourceLocation locationFromRange(const AST::SourceLocation &start,
                                                   const AST::SourceLocation &end);

QMLJS_EXPORT AST::SourceLocation fullLocationForQualifiedId(AST::UiQualifiedId *);

QMLJS_EXPORT QString idOfObject(AST::UiObjectDefinition *object);
QMLJS_EXPORT QString idOfObject(AST::UiObjectBinding *object);
QMLJS_EXPORT QString idOfObject(AST::UiObjectInitializer *initializer);

QMLJS_EXPORT AST::UiObjectInitializer *initializerOfObject(AST::Node *node);
QMLJS_EXPORT AST::UiQualifiedId *qualifiedTypeNameId(AST::Node *node);

QMLJS_EXPORT bool isValidBuiltinPropertyType(const QString &name);

QMLJS_EXPORT DiagnosticMessage errorMessage(const AST::SourceLocation &loc,
                                            const QString &message);

template <class T>
DiagnosticMessage errorMessage(const T *node, const QString &message)
{
    return DiagnosticMessage(DiagnosticMessage::Error,
                             locationFromRange(node->firstSourceLocation(),
                                               node->lastSourceLocation()),
                             message);
}

} // namespace QmlJS

#endif // QMLJS_QMLJSUTILS_H
