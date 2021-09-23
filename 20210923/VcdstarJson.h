#ifndef VCDSTAR_JSON_H__
#include <string>
#include <vector>

namespace vcdstar
{
	enum JsonType {
		TypeNull = 0,
		TypeInt,
		TypeDouble,
		TypeString,
		TypeBool,
		TypeArray,
		TypeObject
	};

	class VcdstarJson
	{
	public:
		VcdstarJson();
		~VcdstarJson();
		VcdstarJson(const VcdstarJson& jsonObj);
		VcdstarJson(const VcdstarJson* jsonObj);
		VcdstarJson(VcdstarJson&& jsonObj);
		VcdstarJson operator=(const VcdstarJson& jsonObj);
		VcdstarJson operator=(const VcdstarJson* jsonObj);
		VcdstarJson operator=(VcdstarJson&& jsonObj);
		VcdstarJson operator=(const int& iVal);
		VcdstarJson operator=(const double& dVal);
		VcdstarJson operator=(const long long& llVal);
		VcdstarJson operator=(const bool& bVal);
		VcdstarJson operator=(const std::string& strVal);
		VcdstarJson operator=(const char* strVal);

		bool LoadPath(const std::string& strPath);
		bool LoadString(const std::string& strJson);
		bool ParseJson(const std::string& strJson);
		std::string ToString(std::string prefix = "", std::string indent = "");

		int GetType();
		std::string GetKey();
		int GetCount();
		VcdstarJson& operator[](const char* key);
		VcdstarJson& operator[](const std::string& key);
		VcdstarJson& operator[](const int key);
		operator bool();
		operator int();
		operator long long();
		operator double();
		operator std::string();
		operator const char* ();
		bool GetBool();
		int GetInt();
		long long GetLong();
		double GetDouble();
		std::string GetString();

		bool RemoveNode(const std::string& key);
		bool RemoveNode(const int& key);

		bool AddNull();
		bool Add(const bool& bVal);
		bool Add(const int& iVal);
		bool Add(const long long& llVal);
		bool Add(const double& dVal);
		bool Add(const std::string& strVal);
		bool Add(const char* strVal);
		bool Add(const VcdstarJson& jsonObj);
		bool InsertNull(const int& position);
		bool Insert(const int& position, const bool& bVal);
		bool Insert(const int& position, const int& iVal);
		bool Insert(const int& position, const long long& llVal);
		bool Insert(const int& position, const double& dVal);
		bool Insert(const int& position, const std::string& strVal);
		bool Insert(const int& position, const char* strVal);
		bool Insert(const int& position, const VcdstarJson& jsonObj);

		bool AddNull(const std::string& strKey);
		bool Add(const std::string& strKey, const bool& bVal);
		bool Add(const std::string& strKey, const int& iVal);
		bool Add(const std::string& strKey, const long long& llVal);
		bool Add(const std::string& strKey, const double& dVal);
		bool Add(const std::string& strKey, const std::string& strVal);
		bool Add(const std::string& strKey, const char* strVal);
		bool Add(const std::string& strKey, const VcdstarJson& jsonObj);
	private:
		void ReleaseThis();
		bool ParseJson_(const std::string& strJson);
		int ParseObject(const std::string& strJson, int iIndex, std::vector<VcdstarJson*>* obj);
		int ParseArray(const std::string& strJson, int iIndex, std::vector<VcdstarJson*>* obj);
		int ParseValue(const std::string& strJson, int iIndex, VcdstarJson* jsonObj);
		int ParseString(const std::string& strJson, int iIndex, std::string* jsonStr);
		int ParseNumber(const std::string& strJson, int iIndex, VcdstarJson* jsonObj);
		int JumpSpace(const std::string& strJson, int iIndex);

		std::string JosnObjectToString(std::vector<VcdstarJson*>* obj, int deep, std::string prefix = "", std::string indent = "");
		std::string JosnArrayToString(std::vector<VcdstarJson*>* obj, int deep, std::string prefix = "", std::string indent = "");

		bool AddJsonNode(VcdstarJson* jsonObj, int iType);
		bool InsertJsonNode(const int& Position, VcdstarJson* jsonObj);

		void CopyJson(const VcdstarJson* const src, VcdstarJson* dst);
	private:
		std::string			m_strKey;
		union
		{
			long long					m_iVal;
			std::string*				m_strVal;
			bool						m_bVal;
			double						m_dVal;
			std::vector<VcdstarJson*>*	m_LstVal;
		};
		int					m_iType;
		char test[81920];
	};
}

#define VCDSTAR_JSON_H__
#endif // !VCDSTAR_JSON_H__