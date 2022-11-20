#include "moduleconfiguration.h"

ModuleConfiguration::ModuleConfiguration(const QString &descriptor,
                                         QObject *parent)
    : QObject{parent} {
  initFromDescription(descriptor);
}

void ModuleConfiguration::initFromDescription(const QString &descr) {}
