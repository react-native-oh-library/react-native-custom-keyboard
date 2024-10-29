// NOTE: This entire file should be codegen'ed.
#include "CustomKeyboardTurboModuleSpec.h"

using namespace rnoh;
using namespace facebook;

static constexpr double HEIGHT = 216;
static constexpr int AVOIDENCE = 0;
static constexpr int START_SURFACE_ID=45;
static constexpr int CLICK_TARGET_ID=NODE_ON_CLICK+101;
NativeCustomKeyboardTurboModuleSpecJSI::~NativeCustomKeyboardTurboModuleSpecJSI() {
        for (const UserData *userData : m_userDatas) {
            delete userData;
            userData = nullptr;
        }
    }
TextInputNode *NativeCustomKeyboardTurboModuleSpecJSI::getEditById(int tag) {
    auto weakInstance = m_ctx.instance;
    auto instance = weakInstance.lock();
    if (!instance) {
        return nullptr;
    }
    auto instanceCapi = std::dynamic_pointer_cast<RNInstanceCAPI>(instance);
    if (!instanceCapi) {
        return nullptr;
    }
    auto componentInstance = instanceCapi->findComponentInstanceByTag(tag);
    if (!componentInstance) {
        return nullptr;
    }
    auto input = std::dynamic_pointer_cast<TextInputComponentInstance>(componentInstance);
    if (!input) {
        return nullptr;
    }
    ArkUINode &node = input->getLocalRootArkUINode();
    TextInputNode *textInputNode = dynamic_cast<TextInputNode *>(&node);
    return textInputNode;
}

static std::shared_ptr<RNInstanceCAPI> getInstanceCapi(std::weak_ptr<RNInstance> weakInstance) {
    auto instance = weakInstance.lock();
    if (!instance) {
        return nullptr;
    }
    auto instanceCapi = std::dynamic_pointer_cast<RNInstanceCAPI>(instance);
    if (!instanceCapi) {
        return nullptr;
    }
    return instanceCapi;
}

static void getDevicePhycicalPixels(std::shared_ptr<RNInstanceCAPI> instanceCapi, double &width, double &height,
                                    double &scale) {
    auto turboModule = instanceCapi->getTurboModule("DeviceInfo");
    auto deviceInfoTurboModule = std::dynamic_pointer_cast<DeviceInfoTurboModule>(turboModule);
    auto displayMetrics = deviceInfoTurboModule->callSync("getConstants", {});
    if (displayMetrics.count("Dimensions") != 0) {
        auto Dimensions = displayMetrics.at("Dimensions");
        auto screenPhysicalPixels = Dimensions.at("screenPhysicalPixels");
        width = screenPhysicalPixels.at("width").asDouble();
        height = screenPhysicalPixels.at("height").asDouble();
        scale = screenPhysicalPixels.at("scale").asDouble();
    }
}

static void setTextInputUserData(TextInputNode *textInput, UserData *userData) {
    ArkUI_NodeHandle view = textInput->getArkUINodeHandle();
    NativeNodeApi::getInstance()->setUserData(view, userData);
}

static UserData *getTextInputUserData(TextInputNode *textInput) {
    ArkUI_NodeHandle view = textInput->getArkUINodeHandle();
    auto data=(NativeNodeApi::getInstance()->getUserData(view));
    UserData *userData = reinterpret_cast<UserData*>(data);
    return userData;
}

static void getTextSelection(TextInputNode* textInput, int32_t &start, int32_t &end) {
   const ArkUI_AttributeItem* item =
   NativeNodeApi::getInstance()->getAttribute(textInput->getArkUINodeHandle(), NODE_TEXT_INPUT_TEXT_SELECTION);
   start = item->value[0].i32;
   end = item->value[1].i32;
}

static void setCustomKeyboard(TextInputNode *textInput, ArkUI_Node* CustomKeyboard, int32_t const &avoidence){
  ArkUI_NumberValue value = {.i32 = avoidence};
  ArkUI_AttributeItem item{.object=CustomKeyboard,.value=&value,.size=1};
  NativeNodeApi::getInstance()->setAttribute(
  textInput->getArkUINodeHandle(), NODE_TEXT_INPUT_CUSTOM_KEYBOARD, &item);
}

void NativeCustomKeyboardTurboModuleSpecJSI::install(jsi::Runtime &rt, int32_t tag, std::string type) {
    this->m_ctx.taskExecutor->runTask(TaskThread::MAIN, [this, tag, type]() {
        auto edit = getEditById(tag);
        if (!edit) {
            return;
        }
        auto instanceCapi = getInstanceCapi(this->m_ctx.instance);
        if (!instanceCapi) {
            return;
        }
        static int surfaceID = START_SURFACE_ID;
        while(instanceCapi->findComponentInstanceByTag(surfaceID)){surfaceID++;}
        instanceCapi->createSurface(surfaceID, "CustomKeyboard");
        folly::dynamic initialProps = folly::dynamic::object("tag", tag)("type", type);
        double width, height, scale;
        getDevicePhycicalPixels(instanceCapi, width, height, scale);
        instanceCapi->startSurface(surfaceID, 0.0f, 0.0f, width / scale, HEIGHT, 0.0f, 0.0f, scale, false,
                                   std::move(initialProps));
        auto comp = instanceCapi->findComponentInstanceByTag(surfaceID);
        if (!comp) {
            return;
        }
        ArkUINode &node = comp->getLocalRootArkUINode();
        ArkUI_NodeHandle uiNode = node.getArkUINodeHandle();
        //空值 说明edit没有绑定点击事件
        if(!getTextInputUserData(edit)) {
                NativeNodeApi::getInstance()->registerNodeEvent(
                edit->getArkUINodeHandle(), NODE_ON_CLICK, CLICK_TARGET_ID, edit->getArkUINodeHandle());
                auto callback = [](ArkUI_NodeEvent *event) {
                if (OH_ArkUI_NodeEvent_GetTargetId(event) == CLICK_TARGET_ID) {
                //get textInputNode and userData
                ArkUI_NodeHandle textNode=OH_ArkUI_NodeEvent_GetNodeHandle(event);
                void* data=(NativeNodeApi::getInstance()->getUserData(textNode));
                UserData *userData = reinterpret_cast<UserData*>(data);
                if (userData && userData->data && userData->enabled == false) {
                userData->enabled = true;
                //set customkeyboard
                ArkUI_NumberValue value = {.i32 = AVOIDENCE};
                ArkUI_AttributeItem item{.object=userData->data,.value=&value,.size=1};
                NativeNodeApi::getInstance()->setAttribute(
                textNode, NODE_TEXT_INPUT_CUSTOM_KEYBOARD, &item);
        }
            }
        };
        NativeNodeApi::getInstance()->addNodeEventReceiver(edit->getArkUINodeHandle(),callback);            
        }
        
        setCustomKeyboard(edit, uiNode, AVOIDENCE);
        UserData *userData = new UserData({.data = uiNode, .enabled = true});
        this->m_userDatas.insert(userData);
        setTextInputUserData(edit, userData);
    });
}

void NativeCustomKeyboardTurboModuleSpecJSI::uninstall(jsi::Runtime &rt, int32_t tag) {
    this->m_ctx.taskExecutor->runTask(TaskThread::MAIN, [this, tag]() {
        auto instanceCapi = getInstanceCapi(this->m_ctx.instance);
        if (!instanceCapi) {
            return;
        }
        auto edit = getEditById(tag);
        edit->setFocusStatus(false);
        auto error_code =
        NativeNodeApi::getInstance()->resetAttribute(edit->getArkUINodeHandle(), NODE_TEXT_INPUT_CUSTOM_KEYBOARD);
        if(error_code){
            DLOG(ERROR)<<"uninstall custom keyboard error"<<std::endl;
            return;
        }
        edit->setFocusStatus(true);
        UserData *userData = getTextInputUserData(edit);
        userData->data=nullptr;
    });
}

void NativeCustomKeyboardTurboModuleSpecJSI::insertText(jsi::Runtime &rt, int32_t tag, std::string text) {
    this->m_ctx.taskExecutor->runTask(TaskThread::MAIN, [this, tag, text]() {
        TextInputNode *textInputNode = this->getEditById(tag);
        int start = 0;
        int end = 0;
        getTextSelection(textInputNode, start, end);
        std::string content = textInputNode->getTextContent();
        content.replace(start, end - start, text);
        textInputNode->setTextContent(content);
        textInputNode->setTextSelection(start+text.size(), start+text.size());
    });
}
void NativeCustomKeyboardTurboModuleSpecJSI::backSpace(jsi::Runtime &rt, int32_t tag) {
    this->m_ctx.taskExecutor->runTask(TaskThread::MAIN, [this, tag]() {
        TextInputNode *textInputNode = this->getEditById(tag);
        int start = 0;
        int end = 0;
        getTextSelection(textInputNode, start, end);
        std::string content = textInputNode->getTextContent();
        if (start != end) {
            content.replace(start, end - start, "");
            textInputNode->setTextContent(content);
            textInputNode->setTextSelection(start, start);
        } else if (start != 0) {
            content.erase(content.cbegin() + start - 1);
            textInputNode->setTextContent(content);
            textInputNode->setTextSelection(start - 1, start - 1);
        }
    });
}
void NativeCustomKeyboardTurboModuleSpecJSI::doDelete(jsi::Runtime &rt, int32_t tag) {
    this->m_ctx.taskExecutor->runTask(TaskThread::MAIN, [this, tag]() {
        TextInputNode *textInputNode = this->getEditById(tag);
        int start = 0;
        int end = 0;
        getTextSelection(textInputNode, start, end);
        std::string content = textInputNode->getTextContent();
        if (start != end) {
            content.replace(start, end - start, "");
            textInputNode->setTextContent(content);
            textInputNode->setTextSelection(start, start);
        } else if (end != content.size()) {
            content.erase(content.cbegin() + end);
            textInputNode->setTextContent(content);
            textInputNode->setTextSelection(end, end);
        }
    });
}
void NativeCustomKeyboardTurboModuleSpecJSI::moveLeft(jsi::Runtime &rt, int32_t tag) {
    this->m_ctx.taskExecutor->runTask(TaskThread::MAIN, [this, tag]() {
        TextInputNode *textInputNode = this->getEditById(tag);
        int start = 0;
        int end = 0;
        getTextSelection(textInputNode, start, end);
        if (start != end) {
            textInputNode->setTextSelection(start, start);
        } else if (start != 0) {
            textInputNode->setTextSelection(start - 1, start - 1);
        }
    });
}
void NativeCustomKeyboardTurboModuleSpecJSI::moveRight(jsi::Runtime &rt, int32_t tag) {
    this->m_ctx.taskExecutor->runTask(TaskThread::MAIN, [this, tag]() {
        TextInputNode *textInputNode = this->getEditById(tag);
        int start = 0;
        int end = 0;
        getTextSelection(textInputNode, start, end);
        std::string content = textInputNode->getTextContent();
        if (start != end) {
            textInputNode->setTextSelection(end, end);
        } else if (end != content.size()) {
            textInputNode->setTextSelection(end + 1, end + 1);
        }
    });
}

void NativeCustomKeyboardTurboModuleSpecJSI::switchSystemKeyboard(jsi::Runtime &rt, int32_t tag) {
    this->m_ctx.taskExecutor->runTask(TaskThread::MAIN, [this, tag]() {
        auto instanceCapi = getInstanceCapi(this->m_ctx.instance);
        if (!instanceCapi) {
            return;
        }
        auto edit = getEditById(tag);
        edit->setFocusStatus(false);
        auto error_code =
            NativeNodeApi::getInstance()->resetAttribute(edit->getArkUINodeHandle(), NODE_TEXT_INPUT_CUSTOM_KEYBOARD);
        if(error_code){
            DLOG(ERROR)<<"switch system keyboard error"<<std::endl;
            return;
        }
        edit->setFocusStatus(true);
        UserData *userData = getTextInputUserData(edit);
        userData->enabled = false;
    });
}

// 各个方法的宿主函数定义
static jsi::Value __hostFunction_NativeCustomKeyboardTurboModuleSpecJSI_install(jsi::Runtime &rt,
                                                                                react::TurboModule &turboModule,
                                                                                const jsi::Value *args, size_t count) {
    static_cast<NativeCustomKeyboardTurboModuleSpecJSI *>(&turboModule)
        ->install(rt, args[0].asNumber(), args[1].asString(rt).utf8(rt));
    return jsi::Value::undefined();
}

static jsi::Value __hostFunction_NativeCustomKeyboardTurboModuleSpecJSI_uninstall(jsi::Runtime &rt,
                                                                                  react::TurboModule &turboModule,
                                                                                  const jsi::Value *args,
                                                                                  size_t count) {
    static_cast<NativeCustomKeyboardTurboModuleSpecJSI *>(&turboModule)->uninstall(rt, args[0].asNumber());
    return jsi::Value::undefined();
}

static jsi::Value __hostFunction_NativeCustomKeyboardTurboModuleSpecJSI_insertText(jsi::Runtime &rt,
                                                                                   react::TurboModule &turboModule,
                                                                                   const jsi::Value *args,
                                                                                   size_t count) {
    static_cast<NativeCustomKeyboardTurboModuleSpecJSI *>(&turboModule)
        ->insertText(rt, args[0].asNumber(), args[1].asString(rt).utf8(rt));
    return jsi::Value::undefined();
}

static jsi::Value __hostFunction_NativeCustomKeyboardTurboModuleSpecJSI_backSpace(jsi::Runtime &rt,
                                                                                  react::TurboModule &turboModule,
                                                                                  const jsi::Value *args,
                                                                                  size_t count) {
    static_cast<NativeCustomKeyboardTurboModuleSpecJSI *>(&turboModule)->backSpace(rt, args[0].asNumber());
    return jsi::Value::undefined();
}

static jsi::Value __hostFunction_NativeCustomKeyboardTurboModuleSpecJSI_doDelete(jsi::Runtime &rt,
                                                                                 react::TurboModule &turboModule,
                                                                                 const jsi::Value *args, size_t count) {
    static_cast<NativeCustomKeyboardTurboModuleSpecJSI *>(&turboModule)->doDelete(rt, args[0].asNumber());
    return jsi::Value::undefined();
}

static jsi::Value __hostFunction_NativeCustomKeyboardTurboModuleSpecJSI_moveLeft(jsi::Runtime &rt,
                                                                                 react::TurboModule &turboModule,
                                                                                 const jsi::Value *args, size_t count) {
    static_cast<NativeCustomKeyboardTurboModuleSpecJSI *>(&turboModule)->moveLeft(rt, args[0].asNumber());
    return jsi::Value::undefined();
}

static jsi::Value __hostFunction_NativeCustomKeyboardTurboModuleSpecJSI_moveRight(jsi::Runtime &rt,
                                                                                  react::TurboModule &turboModule,
                                                                                  const jsi::Value *args,
                                                                                  size_t count) {
    static_cast<NativeCustomKeyboardTurboModuleSpecJSI *>(&turboModule)->moveRight(rt, args[0].asNumber());
    return jsi::Value::undefined();
}

static jsi::Value __hostFunction_NativeCustomKeyboardTurboModuleSpecJSI_switchSystemKeyboard(
    jsi::Runtime &rt, react::TurboModule &turboModule, const jsi::Value *args, size_t count) {
    static_cast<NativeCustomKeyboardTurboModuleSpecJSI *>(&turboModule)->switchSystemKeyboard(rt, args[0].asNumber());
    return jsi::Value::undefined();
}

static jsi::Value __hostFunction_NativeCustomKeyboardTurboModuleSpecJSI_displayRNOHError(
    jsi::Runtime &rt, react::TurboModule &turboModule, const jsi::Value *args, size_t count) {
    return jsi::Value(static_cast<ArkTSTurboModule &>(turboModule).call(rt, "displayRNOHError", args, count));
}

static jsi::Value __hostFunction_NativeCustomKeyboardTurboModuleSpecJSI_throwExceptionCpp(
    jsi::Runtime &rt, react::TurboModule &turboModule, const jsi::Value *args, size_t count) {
    throw std::runtime_error("Exception thrown from C++");
}

static jsi::Value __hostFunction_NativeCustomKeyboardTurboModuleSpecJSI_throwExceptionArk(
    jsi::Runtime &rt, react::TurboModule &turboModule, const jsi::Value *args, size_t count) {
    return jsi::Value(static_cast<ArkTSTurboModule &>(turboModule).call(rt, "throwExceptionArk", args, count));
}

NativeCustomKeyboardTurboModuleSpecJSI::NativeCustomKeyboardTurboModuleSpecJSI(const ArkTSTurboModule::Context ctx,
                                                                               const std::string name)
    : ArkTSTurboModule(ctx, name) {
    methodMap_ = {};

    methodMap_["install"] = MethodMetadata{2, __hostFunction_NativeCustomKeyboardTurboModuleSpecJSI_install};
    methodMap_["uninstall"] = MethodMetadata{1, __hostFunction_NativeCustomKeyboardTurboModuleSpecJSI_uninstall};
    methodMap_["insertText"] = MethodMetadata{2, __hostFunction_NativeCustomKeyboardTurboModuleSpecJSI_insertText};
    methodMap_["backSpace"] = MethodMetadata{1, __hostFunction_NativeCustomKeyboardTurboModuleSpecJSI_backSpace};
    methodMap_["doDelete"] = MethodMetadata{1, __hostFunction_NativeCustomKeyboardTurboModuleSpecJSI_doDelete};
    methodMap_["moveLeft"] = MethodMetadata{1, __hostFunction_NativeCustomKeyboardTurboModuleSpecJSI_moveLeft};
    methodMap_["moveRight"] = MethodMetadata{1, __hostFunction_NativeCustomKeyboardTurboModuleSpecJSI_moveRight};
    methodMap_["switchSystemKeyboard"] =
        MethodMetadata{1, __hostFunction_NativeCustomKeyboardTurboModuleSpecJSI_switchSystemKeyboard};

    methodMap_["displayRNOHError"] =
        MethodMetadata{1, __hostFunction_NativeCustomKeyboardTurboModuleSpecJSI_displayRNOHError};
    methodMap_["throwExceptionCpp"] =
        MethodMetadata{0, __hostFunction_NativeCustomKeyboardTurboModuleSpecJSI_throwExceptionCpp};
    methodMap_["throwExceptionArk"] =
        MethodMetadata{0, __hostFunction_NativeCustomKeyboardTurboModuleSpecJSI_throwExceptionArk};
}