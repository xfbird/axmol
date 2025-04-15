#include <sstream>

#include "UserData.h"
#include "yasio/byte_buffer.hpp"  // 假设byte_buffer需要此头文件


namespace ax {

namespace {
constexpr char RECORD_SEPARATOR = '\x1E';
constexpr char UNIT_SEPARATOR = '\x1F';
}  // namespace

UserData* UserData::create(std::string_view aStorageName) {
  return new UserData(aStorageName);
}

UserData::UserData(std::string_view name) : _storageName(name) {
  auto userDefault = UserDefault::getInstance();
  std::string encoded = userDefault->getStringForKey(_storageName.c_str(), "");
  if (!encoded.empty()) {
    // 使用新的base64解码接口
    yasio::byte_buffer decodedBuf = base64Decode(encoded);
    deserialize(std::string_view(
        reinterpret_cast<const char*>(decodedBuf.data()), decodedBuf.size()));
  }
}

void UserData::setStringForKey(std::string_view key, std::string_view value) {
  _dataMap[std::string(key)] = std::string(value);
}

std::string UserData::getStringForKey(std::string_view key,
                                      std::string_view defaultValue) {
  auto it = _dataMap.find(key);
  if (it == _dataMap.end() || it->second.empty()) {
    std::string defaultStr(defaultValue);
    setStringForKey(key, defaultStr);
    return defaultStr;
  }
  return it->second;
}

std::string UserData::serialize() const {
  std::ostringstream oss;
  bool first = true;

  for (const auto& pair : _dataMap) {
    if (!first) oss << RECORD_SEPARATOR;
    oss << pair.first << UNIT_SEPARATOR << pair.second;
    first = false;
  }
  const std::string& data = oss.str();
  // 使用新的base64编码接口
  return base64Encode(data.data(), data.size());
}

void UserData::deserialize(std::string_view data) {
  _dataMap.clear();
  size_t pos = 0;

  while (pos < data.size()) {
    size_t recordEnd = data.find(RECORD_SEPARATOR, pos);
    if (recordEnd == std::string_view::npos) recordEnd = data.size();

    std::string_view record = data.substr(pos, recordEnd - pos);
    size_t sepPos = record.find(UNIT_SEPARATOR);

    if (sepPos != std::string_view::npos) {
      std::string key(record.substr(0, sepPos));
      std::string value(record.substr(sepPos + 1));
      _dataMap.emplace(std::move(key), std::move(value));
    }
    pos = recordEnd + 1;
  }
}

void UserData::writeMapData() {
  auto userDefault = UserDefault::getInstance();
  userDefault->setStringForKey(_storageName.c_str(), serialize());
  userDefault->flush();
}

// 类型转换方法实现
void UserData::setDoubleForKey(std::string_view key, double value) {
  std::ostringstream oss;
  oss << value;
  setStringForKey(key, oss.str());
}

double UserData::getDoubleForKey(std::string_view key, double defaultValue) {
  std::string str = getStringForKey(key);
  return str.empty() ? defaultValue : std::stod(str);
}

void UserData::setFloatForKey(std::string_view key, float value) {
  std::ostringstream oss;
  oss << value;
  setStringForKey(key, oss.str());
}

float UserData::getFloatForKey(std::string_view key, float defaultValue) {
  std::string str = getStringForKey(key);
  return str.empty() ? defaultValue : std::stof(str);
}

void UserData::setBoolForKey(std::string_view key, bool value) {
  setStringForKey(key, value ? "true" : "false");
}

bool UserData::getBoolForKey(std::string_view key, bool defaultValue) {
  std::string str = getStringForKey(key);
  if (str == "true") return true;
  if (str == "false") return false;
  return defaultValue;
}

void UserData::setInt64ForKey(std::string_view key, int64_t value) {
  setStringForKey(key, std::to_string(value));
}

int64_t UserData::getInt64ForKey(std::string_view key, int64_t defaultValue) {
  std::string str = getStringForKey(key);
  return str.empty() ? defaultValue : std::stoll(str);
}

void UserData::setIntegerForKey(std::string_view key, int value) {
  setStringForKey(key, std::to_string(value));
}

int UserData::getIntegerForKey(std::string_view key, int defaultValue) {
  std::string str = getStringForKey(key);
  return str.empty() ? defaultValue : std::stoi(str);
}

}  // namespace ax