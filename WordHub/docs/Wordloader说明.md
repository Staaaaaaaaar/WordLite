# Wordloader 类文档

**负责人**: [刘继轩](https://github.com/stibiums)

## 一、概述
`Wordloader` 类是一个工具类，负责从不同格式的外部文件（如 `.txt`, `.json`）中解析单词数据，并将其导入到指定的 `WordDatabase` 数据库中。它是实现自定义词库功能的关键模块。

## 二、设计理念
`Wordloader` 扮演着“数据转换器”的角色。
*   **设计理念**: 它的职责单一且明确：将外部不同格式的数据源（文件）转换为程序内部统一的 `Word` 对象，并利用 `WordDatabase` 的接口将其持久化。这种设计将复杂、易变的文件解析逻辑与核心的数据库操作逻辑分离开，使得未来支持更多文件格式（如XML, CSV）变得简单，只需增加新的导入方法即可，无需改动其他模块。

## 三、方法
- `bool importWordsFromJson(const QString& jsonFilePath, const QString& dbName)`: 从一个特定格式的 JSON 文件导入单词。
    - **参数**:
        - `jsonFilePath`: JSON 文件的路径。
        - `dbName`: 目标数据库的名称。
    - **返回值**: `bool`，若导入成功返回 `true`。
- `bool importWordFromTXT(const QString& TXTFilePath, const QString& dbName, ..., bool use_API=false)`: 从一个纯文本文件导入单词列表。
    - **参数**:
        - `TXTFilePath`: TXT 文件的路径。
        - `dbName`: 目标数据库的名称。
        - `use_API`: 是否为每个单词调用网络词典 API 来自动补全详细释义。
    - **返回值**: `bool`，若导入成功返回 `true`。

## 四、私有实现
- `QVector<Phonetic> extractPhonetics(const QJsonObject& phoneticsObj)`: 从 JSON 对象中解析音标信息。
- `QMap<QString, QVector<Definition>> extractMeanings(const QJsonObject& wordInfo)`: 从 JSON 对象中解析多词性释义。
- `QStringList extractWords(const QString& text)`: 使用正则表达式从文本行中提取出单词本身。
- `bool isInvalidLine(const QString& line)`: 使用启发式规则（如检查是否包含数字）过滤掉无效的文本行。