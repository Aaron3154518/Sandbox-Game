#ifndef OBJECT_PARSER_H
#define OBJECT_PARSER_H

#include <stack>
#include <map>
#include <forward_list>
#include <fstream>
#include <exception>

#include "../Utils/Utils.h"

namespace object {
	// Object parser
	struct TileNotFoundException : public std::exception {
		std::string message = "";

		TileNotFoundException(TileId id) {
			message = "Tile Not Found: ";
			message.append(std::to_string(id));
		}

		const char* what() const throw () {
			return message.c_str();
		}
	};

	struct MissingObjectNameException : public std::exception {
		const char* what() const throw () {
			return "Expected data object name";
		}
	};

	struct MissingDataNameException : public std::exception {
		const char* what() const throw () {
			return "Expected data name";
		}
	};

	struct MissingDataValueException : public std::exception {
		const char* what() const throw () {
			return "Expected data value";
		}
	};

	struct UnclosedBracesException : public std::exception {
		const char* what() const throw () {
			return "Expected '}'";
		}
	};

	struct DataObject {
		std::string name = "";
		std::map<std::string, DataObject> objs;
		std::map<std::string, bool> bools;
		std::map<std::string, double> nums;
		std::map<std::string, std::string> strs;

		void addDataField(std::string name, std::string val);

		DataObject getDataObject(std::string name) const;
		bool getBool(std::string name, bool default = true) const;
		double getNum(std::string name, double default = 0.) const;
		std::string getStr(std:string name, std::string default = "") const;

		void print() const;
	};

	static std::forward_list<DataObject> parseObjectData(std::string fileName);

	// Object Loader
	struct InvalidIDException : public std::exception {
		std::string message = "";

		InvalidIDException(int id) {
			message = "Invalid ID: ";
			message.append(std::to_string(id));
		}

		const char* what() const throw () {
			return message.c_str();
		}

	};

	struct InvalidNameException : public std::exception {
		std::string message = "";

		InvalidIDException(std::string name) {
			message = "Invalid object name: ";
			message.append(name);
		}

		const char* what() const throw () {
			return message.c_str();
		}

	};

	struct NoLoadableTypesExceptions {
		const char* what() const throw () {
			return "Expected looadable type, found none";
		}
	};

	struct LoaderInfo {
		std::string file, defType;
		std::string objNames[];
		std::map<std::string, std::type_index> types;

		std::type_index getObjectType(std::string name);
		int getObjectIndex(std::string name);
	};

	// TId must be enum
	template<T, TId,
		typename std::enable_if<std::is_enum<TId>::value>::type* = nullptr>
		class Loader {
		public:
			Loader() = default;
			Loader(int size, LoaderInfo & _info) : info(_info) { object.resize(size); }
			~Loader() = default;

			T* addObject(TId id, T* obj);
			void loadObjects();
			T* getObject(TId id);
			void cleanObjects();

			TId getId(std::string name);

		private:
			LoaderInfo info;
			std::vector<T*> objects;
	};

}
#endif