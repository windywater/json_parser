# json parser
ģ�� jsoncpp д��һ�� json parser��

## �÷�
```C++
	json::Reader reader;
	json::Value root;
	bool ok = reader.parse("{\"name\": \"windy\", age: 30}", root);
	root["sex"] = "male";
	json::Writer writer;
	cout << writer.writeValue(root);
```
