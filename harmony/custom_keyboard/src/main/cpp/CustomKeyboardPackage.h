#pragma once
#include "RNOH/Package.h"

namespace rnoh {
class CustomKeyboardPackage : public Package {

public:
    CustomKeyboardPackage(Package::Context ctx) : Package(ctx) {}

    std::unique_ptr<TurboModuleFactoryDelegate> createTurboModuleFactoryDelegate() override;
};
} // namespace rnoh