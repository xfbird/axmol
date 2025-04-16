#pragma once

#include <map>
#include <string>
#include <string_view>

#include "base/UserDefault.h"
#include "base/Utils.h"  // 包含新的base64头文件

namespace ax {

class UserData : public Object  {
 public:
  static UserData* create(std::string_view aStorageName);

  void setStringForKey(std::string_view key, std::string_view value);
  std::string getStringForKey(std::string_view key,
                              std::string_view defaultValue = "");

  // 类型转换方法声明
  void setDoubleForKey(std::string_view key, double value);
  void setFloatForKey(std::string_view key, float value);
  void setBoolForKey(std::string_view key, bool value);
  void setInt64ForKey(std::string_view key, int64_t value);
  void setIntegerForKey(std::string_view key, int value);

  double getDoubleForKey(std::string_view key, double defaultValue = 0.0);
  float getFloatForKey(std::string_view key, float defaultValue = 0.0f);
  bool getBoolForKey(std::string_view key, bool defaultValue = false);
  int64_t getInt64ForKey(std::string_view key, int64_t defaultValue = 0);
  int getIntegerForKey(std::string_view key, int defaultValue = 0);

  void writeMapData();

 private:
  UserData(std::string_view name);

  std::string serialize() const;
  void deserialize(std::string_view data);

  std::string _storageName;
  std::map<std::string, std::string, std::less<>> _dataMap;
};

}  // namespace ax