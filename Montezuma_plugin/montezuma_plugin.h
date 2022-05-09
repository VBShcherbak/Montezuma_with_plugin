#ifndef MONTEZUMA_PLUGIN_H
#define MONTEZUMA_PLUGIN_H
#include <QQmlExtensionPlugin>

class MontezumaPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char *uri) override;
};

#endif // MONTEZUMA_PLUGIN_H
