#include "montezuma_plugin.h"
#include "montezuma.h"
#include <qqml.h>

void MontezumaPlugin::registerTypes(const char *uri)
{
    // @uri task.two.qml.cpp
    qmlRegisterType<Montezuma>(uri, 1, 0, "Montezuma");
}

