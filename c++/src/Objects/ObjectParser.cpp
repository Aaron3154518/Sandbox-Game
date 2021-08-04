#include "ObjectParser.h"

namespace object {
	// Data Object
	void DataObject::addDataField(std::string name, std::string val) {
		// Create a lowercase copy
		std::string tmp = utils::lowerCase(val);
		// Check for true/false
		if (tmp == "true") { bools[name] = true; }
		else if (tmp == "false") { bools[name] = false; }
		else {
			// Try to convert to decimal
			double dVal;
			if (utils::strToDouble(val, dVal)) { nums[name] = dVal; }
			else { strs[name] = val; }
		}
	}

	DataObject getDataObject(std::string name) const {
		auto it = objs.find(name);
		return it != objs.end() ? it->second : DataObject();
	}
	bool getBool(std::string name, bool default) const {
		auto it = bools.find(name);
		return it != bools.end() ? it->second : default;
	}
	double getNum(std::string name, double default) const {
		auto it = nums.find(name);
		return it != nums.end() ? it->second : default;
	}
	std::string getStr(std:string name, std::string default) const {
		auto it = strs.find(name);
		return it != strs.end() ? it->second : default;

	}

	void DataObject::print() const {
		std::cerr << "\nData Object: " << name << std::endl;
		if (!bools.empty()) {
			std::cerr << "Booleans: {" << std::endl;
			for (auto pair : bools) {
				std::cerr << "\t" << pair.first << " : "
					<< (pair.second ? "true" : "false") << "," << std::endl;
			}
			std::cerr << "}" << std::endl;
		}
		if (!nums.empty()) {
			std::cerr << "Numbers: {" << std::endl;
			for (auto pair : nums) {
				std::cerr << "\t" << pair.first << " : " << pair.second << ","
					<< std::endl;
			}
			std::cerr << "}" << std::endl;
		}
		if (!strs.empty()) {
			std::cerr << "Strings: {" << std::endl;
			for (auto pair : strs) {
				std::cerr << "\t" << pair.first << " : " << pair.second << ","
					<< std::endl;
			}
			std::cerr << "}" << std::endl;
		}
		if (!objs.empty()) {
			std::cerr << "Data Objects: {" << std::endl;
			for (auto pair : objs) {
				pair.second.print();
			}
			std::cerr << "}" << std::endl;
		}
	}

	std::forward_list<DataObject> parseObjectData(std::string fileName) {
		std::ifstream file(fileName);
		std::forward_list<DataObject> result;
		std::stack<DataObject> objs;
		std::string line, dataName = "";
		bool quotes = false;
		std::ostringstream ss;
		while (std::getline(file, line)) {
			for (auto it = line.begin(); it != line.end(); it++) {
				if (quotes) {
					if (*it == '"') { quotes = false; }
					else { ss << *it; }
					continue;
				}
				switch (*it) {
				case ' ':
				case '\t':
				case '\n':
					break;
				case '"':
					quotes = true;
					break;
				case '{': {
					std::string objName = ss.str();
					ss.str("");
					if (objName == "") { throw MissingObjectNameException(); }
					DataObject obj;
					obj.name = objName;
					objs.push(obj);
					break;
				}
				case '}':
				case ',': {
					if (dataName != "") {
						std::string dataVal = ss.str();
						ss.str("");
						if (dataVal == "") { throw MissingDataValueException(); }
						objs.top().addDataField(dataName, dataVal);
						dataName = "";
					}
					if (*it == '}') {
						DataObject top = objs.top();
						objs.pop();
						if (!objs.empty()) { objs.top().objs[top.name] = top; }
						else { result.push_front(top); }
					}
					break;
				}
				case ':': {
					if (dataName != "") { throw MissingDataValueException(); }
					dataName = ss.str();
					ss.str("");
					if (dataName == "") { throw MissingDataNameException(); }
					break;
				}
				default:
					ss << *it;
					break;
				}
			}
		}
		if (!objs.empty()) { throw UnclosedBracesException(); }
		file.close();
		for (auto o : result) { o.print(); }
		return result;
	}

	// LoaderInfo
	std::type_index LoaderInfo::getObjectType(std::string typeName) {
		auto it = types.find(typeName);
		return it != types.end() ? it->second : types.at(defType);
	}

	// TODO: optimize
	int LoaderInfo::getObjectIndex(std::string name) {
		for (int i = 0; i < TileId::numIds; i++) {
			if (name == objNames[i]) { return i; }
		}
		return -1;
	}

	// Loader
	T* Loader::addObject(TId id, T* obj) {
		if (id >= objects.size()) {
			throw InvalidIDException(id);
		}
		else {
			if (objects[id]) { delete objects[id]; }
			objects[id] = obj;
		}
		return obj;
	}

	void Loader::loadObjects() {
		auto objData = parseObjects(info.file);
		for (DataObject& obj : objData) {
			TId id = getId(obj.name);
			if (id >= objects.size()) {
				throw InvalidIDException(id);
				continue;
			}
			if (!objects[id]) {
				std::string typeName = obj.getStr("Type");
				objects[id] = new info.getObjectType(typeName)();
			}
			objects[id]->load(obj);
		}
	}

	T* Loader::getObject(TId id) {
		if (id >= objects.size()) {
			throw InvalidIDException(id);
		}
		else if (!objects[id]) {
			throw TileNotFoundException(id);
		}
		return objects[id];
	}

	void TileLoader::cleanObjects() {
		for (auto it = objects.begin(); it != objects.end(); it++) {
			if (!(*it)) { continue; }
			delete* it;
			*it = nullptr;
		}
	}

	TileId TileLoader::getId(std::string name) {
		int idx = info.getObjectIndex(name);
		if (idx == -1) {
			throw InvalidNameException(name);
		}
		return static_cast<TId>(idx);
	}
}