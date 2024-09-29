import type { TurboModule } from 'react-native/Libraries/TurboModule/RCTExport';
import { TurboModuleRegistry } from 'react-native';

export interface Spec extends TurboModule {
  install: (tag: number, type: string) => void;
  uninstall: (tag: number) => void;
  insertText: (tag: number, text: string) => void;
  backSpace: (tag: number) => void;
  doDelete: (tag: number) => void;
  moveLeft: (tag: number) => void;
  moveRight: (tag: number) => void;
  switchSystemKeyboard: (tag: number) => void;
}

export default TurboModuleRegistry.get<Spec>('CustomKeyboardNativeModule') as Spec | null;