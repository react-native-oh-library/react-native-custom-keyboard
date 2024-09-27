// NOTE: This entire file should be codegen'ed.
#pragma once

#include <ReactCommon/TurboModule.h>
#include "RNOH/ArkTSTurboModule.h"
#include "RNOH/arkui/TextInputNode.h"
#include "RNOHCorePackage/ComponentInstances/TextInputComponentInstance.h"
#include "RNOH/RNInstanceCAPI.h"
#include "RNOHCorePackage/TurboModules/DeviceInfoTurboModule.h"
using namespace facebook;

namespace rnoh {

typedef struct{
    ArkUI_NodeHandle data;
    bool enabled;
}UserData;

class JSI_EXPORT NativeCustomKeyboardTurboModuleSpecJSI : public ArkTSTurboModule {
public:
    NativeCustomKeyboardTurboModuleSpecJSI(const ArkTSTurboModule::Context ctx, const std::string name);
    ~NativeCustomKeyboardTurboModuleSpecJSI();
    void install(jsi::Runtime &rt, int32_t tag, std::string type);
    void uninstall(jsi::Runtime &rt, int32_t tag);
    void insertText(jsi::Runtime &rt, int32_t tag, std::string text);
    void backSpace(jsi::Runtime &rt, int32_t tag);
    void doDelete(jsi::Runtime &rt, int32_t tag);
    void moveLeft(jsi::Runtime &rt, int32_t tag);
    void moveRight(jsi::Runtime &rt, int32_t tag);
    void switchSystemKeyboard(jsi::Runtime &rt, int32_t tag);

private:
    TextInputNode *getEditById(int id);

private:
    std::unordered_set<UserData *> m_userDatas;
};

} // namespace rnoh