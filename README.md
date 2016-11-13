# json parser
模仿 jsoncpp 写了一个 json parser。

## 用法
```C++
	json::Reader reader;
	json::Value root;
	bool ok = reader.parse("{\"name\": \"windy\", age: 30}", root);
	root["sex"] = "male";
	json::Writer writer;
	cout << writer.writeValue(root);
```
