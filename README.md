# KUBTSS_Boards_25th
KUBTSS Avionics 25th designed by pprk

## 右矢印（->）について / About the Arrow Operators

このリポジトリに表示されている右矢印（`->`）は、C++言語のアロー演算子です。
これは `esp32_ble_connector/esp32_ble_connector.ino` ファイル内で使用されており、
ポインタを通してオブジェクトのメンバ関数やメンバ変数にアクセスするために使用されます。

例：
- `pCharacteristic -> addDescriptor()` - pCharacteristicポインタのaddDescriptorメソッドを呼び出し
- `pService -> start()` - pServiceポインタのstartメソッドを呼び出し

The right arrows (`->`) displayed in this repository are C++ arrow operators.
They are used in the `esp32_ble_connector/esp32_ble_connector.ino` file to access 
member functions and variables of objects through pointers.
