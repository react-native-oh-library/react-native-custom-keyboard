#include "CustomKeyboardPackage.h"
#include "CustomKeyboardTurboModuleSpec.h"

using namespace facebook;
namespace rnoh {
class CustomKeyboardTurboModuleFactoryDelegate : public TurboModuleFactoryDelegate {
public:
    SharedTurboModule createTurboModule(Context ctx, const std::string &name) const override {
        if (name == "CustomKeyboardNativeModule") {
            return std::make_shared<NativeCustomKeyboardTurboModuleSpecJSI>(ctx, name);
        }
        return nullptr;
    };
};

std::unique_ptr<TurboModuleFactoryDelegate> CustomKeyboardPackage::createTurboModuleFactoryDelegate() {
    return std::make_unique<CustomKeyboardTurboModuleFactoryDelegate>();
}
} // namespace rnoh