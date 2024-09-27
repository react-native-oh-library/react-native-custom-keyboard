declare namespace RNCustomKeyboard {
    function install: (tag: number, type: string) => void;
    function uninstall: (tag: number) => void;
    function insertText: (tag: number, text: string) => void;
    function backSpace: (tag: number) => void;
    function doDelete: (tag: number) => void;
    function moveLeft: (tag: number) => void;
    function moveRight: (tag: number) => void;
    function switchSystemKeyboard: (tag: number) => void;
}
export = RNCustomKeyboard;