#include "VcdstarJson.h"
#include <stdio.h>
#include <fstream>
#include <memory>
#include <iostream>

using namespace vcdstar;


VcdstarJson::VcdstarJson()
{
	m_iType = TypeNull;
	m_iVal = 0;
	m_strVal = nullptr;
	m_bVal = false;
	m_dVal = 0;
	m_LstVal = nullptr;
}


VcdstarJson::~VcdstarJson()
{
	ReleaseThis();
}

VcdstarJson::VcdstarJson(const VcdstarJson& jsonObj)
	: VcdstarJson()
{
	CopyJson(&jsonObj, this);
}

VcdstarJson::VcdstarJson(const VcdstarJson* jsonObj)
	: VcdstarJson()
{
	if (jsonObj != nullptr)
		CopyJson(jsonObj, this);
}

VcdstarJson::VcdstarJson(VcdstarJson&& jsonObj)
	: VcdstarJson()
{
	m_strKey = jsonObj.m_strKey;
	m_iType = jsonObj.m_iType;
	if (jsonObj.m_iType == TypeInt)
		m_iVal = jsonObj.m_iVal;
	else if (jsonObj.m_iType == TypeBool)
		m_bVal = jsonObj.m_bVal;
	else if (jsonObj.m_iType == TypeDouble)
		m_dVal = jsonObj.m_dVal;
	else if (jsonObj.m_iType == TypeString)
		m_strVal = jsonObj.m_strVal;
	else if (jsonObj.m_iType == TypeArray || jsonObj.m_iType == TypeObject)
		m_LstVal = jsonObj.m_LstVal;

	jsonObj.m_iType = TypeNull;
}

VcdstarJson VcdstarJson::operator=(const VcdstarJson& jsonObj)
{
	std::string strLastkey = m_strKey;
	ReleaseThis();
	CopyJson(&jsonObj, this);
	m_strKey = strLastkey;
	return *this;
}

VcdstarJson VcdstarJson::operator=(const VcdstarJson* jsonObj)
{
	std::string strLastkey = m_strKey;
	ReleaseThis();
	if (jsonObj != nullptr)
		CopyJson(jsonObj, this);
	m_strKey = strLastkey;
	return *this;
}

VcdstarJson VcdstarJson::operator=(VcdstarJson&& jsonObj)
{
	ReleaseThis();
	m_strKey = jsonObj.m_strKey;
	m_iType = jsonObj.m_iType;
	if (jsonObj.m_iType == TypeInt)
		m_iVal = jsonObj.m_iVal;
	else if (jsonObj.m_iType == TypeBool)
		m_bVal = jsonObj.m_bVal;
	else if (jsonObj.m_iType == TypeDouble)
		m_dVal = jsonObj.m_dVal;
	else if (jsonObj.m_iType == TypeString)
		m_strVal = jsonObj.m_strVal;
	else if (jsonObj.m_iType == TypeArray || jsonObj.m_iType == TypeObject)
		m_LstVal = jsonObj.m_LstVal;

	jsonObj.m_iType = TypeNull;
	return *this;
}

VcdstarJson VcdstarJson::operator=(const int& iVal)
{
	std::string strLastKey = m_strKey;
	ReleaseThis();
	m_iType = TypeInt;
	m_iVal = iVal;
	m_strKey = strLastKey;
	return *this;
}

VcdstarJson VcdstarJson::operator=(const double& dVal)
{
	std::string strLastKey = m_strKey;
	ReleaseThis();
	m_iType = TypeDouble;
	m_dVal = dVal;
	m_strKey = strLastKey;
	return *this;
}

VcdstarJson VcdstarJson::operator=(const long long& llVal)
{
	std::string strLastKey = m_strKey;
	ReleaseThis();
	m_iType = TypeInt;
	m_iVal = llVal;
	m_strKey = strLastKey;
	return *this;
}

VcdstarJson VcdstarJson::operator=(const bool& bVal)
{
	std::string strLastKey = m_strKey;
	ReleaseThis();
	m_iType = TypeBool;
	m_bVal = bVal;
	m_strKey = strLastKey;
	return *this;
}

VcdstarJson VcdstarJson::operator=(const std::string& strVal)
{
	std::string strLastKey = m_strKey;
	ReleaseThis();
	m_iType = TypeString;
	m_strVal = new std::string(strVal);
	m_strKey = strLastKey;
	return *this;
}

VcdstarJson VcdstarJson::operator=(const char* strVal)
{
	std::string strLastKey = m_strKey;
	ReleaseThis();
	m_iType = TypeString;
	m_strVal = new std::string(strVal);
	m_strKey = strLastKey;
	return *this;
}

VcdstarJson VcdstarJson::SetNull()
{
	std::string strLastKey = m_strKey;
	ReleaseThis();
	m_strKey = strLastKey;
	return *this;
}

bool VcdstarJson::LoadPath(const std::string& strPath)
{
	int size = 0;
	std::ifstream fin(strPath);
	if (fin.is_open())
	{
		fin.seekg(0, std::ios::end);
		size = fin.tellg();
		fin.close();
	}
	else
	{
		return false;
	}

	FILE* fp = fopen(strPath.c_str(), "rb");
	if (fp)
	{
		char* szData = new char[size];
		std::shared_ptr<char> szDatad = std::shared_ptr<char>(szData, [](char* d) {delete[] d; });
		fread(szData, 1, size, fp);
		fclose(fp);
		return ParseJson_(szData);
	}
	return false;
}

bool VcdstarJson::LoadJson(const std::string& strJson)
{
	return ParseJson_(strJson);
}

bool VcdstarJson::ParseJson(const std::string& strJson)
{
	return ParseJson_(strJson);
}

std::string VcdstarJson::ToString(std::string prefix, std::string indent)
{
	if (m_iType == TypeArray)
	{
		return JosnArrayToString(m_LstVal, 1, prefix, indent);
	}
	else if (m_iType == TypeObject)
	{
		return JosnObjectToString(m_LstVal, 1, prefix, indent);
	}
	else
	{
		return "";
	}
}

int VcdstarJson::GetType()
{
	return m_iType;
}

std::string VcdstarJson::GetKey()
{
	return m_strKey;
}

int VcdstarJson::GetCount()
{
	if (m_iType == TypeArray || m_iType == TypeObject)
		return m_LstVal->size();
	return -1;
}

VcdstarJson& VcdstarJson::operator[](const char* key)
{
	if (m_iType != TypeObject)
	{
		std::string strLastKey = m_strKey;
		ReleaseThis();
		m_strKey = strLastKey;
		m_iType = TypeObject;
		m_LstVal = new std::vector<VcdstarJson*>();
		VcdstarJson* jsonObj = new VcdstarJson();
		m_LstVal->push_back(jsonObj);
		jsonObj->m_strKey = key;
		return *jsonObj;
	}
	for (std::vector<VcdstarJson*>::iterator it = m_LstVal->begin()
		; it != m_LstVal->end(); it++)
	{
		if ((*it)->m_strKey == std::string(key))
		{
			return **it;
		}
	}

	VcdstarJson* jsonObj = new VcdstarJson();
	m_LstVal->push_back(jsonObj);
	jsonObj->m_strKey = key;
	return *jsonObj;

	return *this;
}

VcdstarJson& VcdstarJson::operator[](const std::string& key)
{
	return (*this)[key.c_str()];
}

VcdstarJson& VcdstarJson::operator[](const int key)
{
	if ((m_iType != TypeObject && m_iType != TypeArray) || key >= m_LstVal->size() || key < 0)
	{
		std::string strLastKey = m_strKey;
		ReleaseThis();
		m_strKey = strLastKey;
		m_iType = TypeArray;
		m_LstVal = new std::vector<VcdstarJson*>();
		VcdstarJson* jsonObj = new VcdstarJson();
		m_LstVal->push_back(jsonObj);
		jsonObj->m_strKey = key;
		return *jsonObj;
	}
	return *(*m_LstVal)[key];
}

VcdstarJson::operator bool()
{
	return GetBool();
}

VcdstarJson::operator int()
{
	return GetInt();
}

VcdstarJson::operator long long()
{
	return GetLong();
}

VcdstarJson::operator double()
{
	return GetDouble();
}

VcdstarJson::operator std::string()
{
	return GetString();
}

VcdstarJson::operator const char* ()
{
	if (m_iType == TypeString)
		return m_strVal->c_str();
	else
		return "";
}

bool VcdstarJson::GetBool()
{
	if (m_iType == TypeBool)
		return m_bVal;
	else
		return false;
}

int VcdstarJson::GetInt()
{
	if (m_iType == TypeInt)
		return m_iVal;
	else
		return 0;
}

long long VcdstarJson::GetLong()
{
	if (m_iType == TypeInt)
		return m_iVal;
	else
		return 0;
}

double VcdstarJson::GetDouble()
{
	if (m_iType == TypeDouble)
		return m_dVal;
	else
		return 0;
}

std::string VcdstarJson::GetString()
{
	if (m_iType == TypeString)
		return *m_strVal;
	else
		return "";
}

bool VcdstarJson::RemoveNode(const std::string& key)
{
	VcdstarJson *jsonObj = nullptr;
	if (m_iType != TypeObject)
		return false;
	for (std::vector<VcdstarJson*>::iterator it = m_LstVal->begin()
		; it != m_LstVal->end(); it++)
	{
		if ((*it)->m_strKey == std::string(key))
		{
			jsonObj = *it;
			m_LstVal->erase(it);
			break;
		}
	}
	if (jsonObj)
	{
		delete jsonObj;
		return true;
	}
	return false;
}

bool VcdstarJson::RemoveNode(const int& key)
{
	if (key >= m_LstVal->size() || key < 0)
		return false;

	VcdstarJson* jsonObj = *(m_LstVal->begin() + key);
	delete jsonObj;
	m_LstVal->erase(m_LstVal->begin() + key);
	return true;
}

bool VcdstarJson::AddNull()
{
	VcdstarJson* jsonObj = new VcdstarJson();
	return AddJsonNode(jsonObj, TypeArray, true);
}

bool VcdstarJson::Add(const bool& bVal)
{
	VcdstarJson* jsonObj = new VcdstarJson();
	jsonObj->m_bVal = bVal;
	jsonObj->m_iType = TypeBool;
	return AddJsonNode(jsonObj, TypeArray, true);
}

bool VcdstarJson::Add(const int& iVal)
{
	VcdstarJson* jsonObj = new VcdstarJson();
	jsonObj->m_iVal = (int)iVal;
	jsonObj->m_iType = TypeInt;
	return AddJsonNode(jsonObj, TypeArray, true);
}

bool VcdstarJson::Add(const long long& llVal)
{
	VcdstarJson* jsonObj = new VcdstarJson();
	jsonObj->m_iVal = llVal;
	jsonObj->m_iType = TypeInt;
	return AddJsonNode(jsonObj, TypeArray, true);
}

bool VcdstarJson::Add(const double& dVal)
{
	VcdstarJson* jsonObj = new VcdstarJson();
	jsonObj->m_dVal = dVal;
	jsonObj->m_iType = TypeDouble;
	return AddJsonNode(jsonObj, TypeArray, true);
}

bool VcdstarJson::Add(const std::string& strVal)
{
	VcdstarJson* jsonObj = new VcdstarJson();
	jsonObj->m_strVal = new std::string(strVal);
	jsonObj->m_iType = TypeString;
	return AddJsonNode(jsonObj, TypeArray, true);
}

bool VcdstarJson::Add(const char* strVal)
{
	VcdstarJson* jsonObj = new VcdstarJson();
	jsonObj->m_strVal = new std::string(strVal);
	jsonObj->m_iType = TypeString;
	return AddJsonNode(jsonObj, TypeArray, true);
}

bool VcdstarJson::Add(const VcdstarJson& jsonObj)
{
	VcdstarJson* jsonObj_ = new VcdstarJson();
	if (m_iType == TypeArray)
	{
		CopyJson(&jsonObj, jsonObj_);
		m_LstVal->push_back(jsonObj_);
		return true;
	}
	else if (m_iType == TypeNull)
	{
		CopyJson(&jsonObj, jsonObj_);
		m_LstVal = new std::vector<VcdstarJson*>();
		m_LstVal->push_back(jsonObj_);
		m_iType = TypeArray;
		return true;
	}
	else
	{
		delete jsonObj_;
	}
	return false;
}

bool VcdstarJson::InsertNull(const int& position)
{
	VcdstarJson* jsonObj = new VcdstarJson();
	return InsertJsonNode(position, jsonObj);
}

bool VcdstarJson::Insert(const int& position, const bool& bVal)
{
	VcdstarJson* jsonObj = new VcdstarJson();
	jsonObj->m_bVal = bVal;
	jsonObj->m_iType = TypeBool;
	return InsertJsonNode(position, jsonObj);
}

bool VcdstarJson::Insert(const int& position, const int& iVal)
{
	VcdstarJson* jsonObj = new VcdstarJson();
	jsonObj->m_iVal = (int)iVal;
	jsonObj->m_iType = TypeInt;
	return InsertJsonNode(position, jsonObj);
}

bool VcdstarJson::Insert(const int& position, const long long& llVal)
{
	VcdstarJson* jsonObj = new VcdstarJson();
	jsonObj->m_iVal = llVal;
	jsonObj->m_iType = TypeInt;
	return InsertJsonNode(position, jsonObj);
}

bool VcdstarJson::Insert(const int& position, const double& dVal)
{
	VcdstarJson* jsonObj = new VcdstarJson();
	jsonObj->m_dVal = dVal;
	jsonObj->m_iType = TypeDouble;
	return InsertJsonNode(position, jsonObj);
}

bool VcdstarJson::Insert(const int& position, const std::string& strVal)
{
	VcdstarJson* jsonObj = new VcdstarJson();
	jsonObj->m_strVal = new std::string(strVal);
	jsonObj->m_iType = TypeString;
	return InsertJsonNode(position, jsonObj);
}

bool VcdstarJson::Insert(const int& position, const char* strVal)
{
	VcdstarJson* jsonObj = new VcdstarJson();
	jsonObj->m_strVal = new std::string(strVal);
	jsonObj->m_iType = TypeString;
	return InsertJsonNode(position, jsonObj);
}

bool VcdstarJson::Insert(const int& position, const VcdstarJson& jsonObj) 
{
	VcdstarJson* jsonObj_ = new VcdstarJson();
	if (m_iType == TypeArray)
	{
		CopyJson(&jsonObj, jsonObj_);
		m_LstVal->insert(m_LstVal->begin() + position, jsonObj_);
		return true;
	}
	else
	{
		delete jsonObj_;
	}
	return false;
}

bool VcdstarJson::AddNull(const std::string& strKey)
{
	VcdstarJson* jsonObj = CheckSame(strKey);
	bool bNeedPush = false;
	if (jsonObj == nullptr) {
		jsonObj = new VcdstarJson();
		bNeedPush = true;
	}
	jsonObj->m_strKey = strKey;
	return AddJsonNode(jsonObj, TypeObject, bNeedPush);
}

bool VcdstarJson::Add(const std::string& strKey, const bool& bVal)
{
	VcdstarJson* jsonObj = CheckSame(strKey);
	bool bNeedPush = false;
	if (jsonObj == nullptr) {
		jsonObj = new VcdstarJson();
		bNeedPush = true;
	}
	jsonObj->m_strKey = strKey;
	jsonObj->m_bVal = bVal;
	jsonObj->m_iType = TypeBool;
	return AddJsonNode(jsonObj, TypeObject, bNeedPush);
}

bool VcdstarJson::Add(const std::string& strKey, const int& iVal)
{
	VcdstarJson* jsonObj = CheckSame(strKey);
	bool bNeedPush = false;
	if (jsonObj == nullptr) {
		jsonObj = new VcdstarJson();
		bNeedPush = true;
	}
	jsonObj->m_strKey = strKey;
	jsonObj->m_iVal = (int)iVal;
	jsonObj->m_iType = TypeInt;
	return AddJsonNode(jsonObj, TypeObject, bNeedPush);
}

bool VcdstarJson::Add(const std::string& strKey, const long long& llVal)
{
	VcdstarJson* jsonObj = CheckSame(strKey);
	bool bNeedPush = false;
	if (jsonObj == nullptr) {
		jsonObj = new VcdstarJson();
		bNeedPush = true;
	}
	jsonObj->m_strKey = strKey;
	jsonObj->m_iVal = llVal;
	jsonObj->m_iType = TypeInt;
	return AddJsonNode(jsonObj, TypeObject, bNeedPush);
}

bool VcdstarJson::Add(const std::string& strKey, const double& dVal)
{
	VcdstarJson* jsonObj = CheckSame(strKey);
	bool bNeedPush = false;
	if (jsonObj == nullptr) {
		jsonObj = new VcdstarJson();
		bNeedPush = true;
	}
	jsonObj->m_strKey = strKey;
	jsonObj->m_dVal = dVal;
	jsonObj->m_iType = TypeDouble;
	return AddJsonNode(jsonObj, TypeObject, bNeedPush);
}

bool VcdstarJson::Add(const std::string& strKey, const std::string& strVal)
{
	VcdstarJson* jsonObj = CheckSame(strKey);
	bool bNeedPush = false;
	if (jsonObj == nullptr) {
		jsonObj = new VcdstarJson();
		bNeedPush = true;
	}
	jsonObj->m_strKey = strKey;
	jsonObj->m_strVal = new std::string(strVal);
	jsonObj->m_iType = TypeString;
	return AddJsonNode(jsonObj, TypeObject, bNeedPush);
}

bool VcdstarJson::Add(const std::string& strKey, const char* strVal)
{
	VcdstarJson* jsonObj = CheckSame(strKey);
	bool bNeedPush = false;
	if (jsonObj == nullptr) {
		jsonObj = new VcdstarJson();
		bNeedPush = true;
	}
	jsonObj->m_strKey = strKey;
	jsonObj->m_strVal = new std::string(strVal);
	jsonObj->m_iType = TypeString;
	return AddJsonNode(jsonObj, TypeObject, bNeedPush);
}

bool VcdstarJson::Add(const std::string& strKey, const VcdstarJson& jsonObj)
{
	VcdstarJson* jsonObj_ = CheckSame(strKey);
	bool bNeedPush = false;
	if (jsonObj_ == nullptr) {
		jsonObj_ = new VcdstarJson();
		bNeedPush = true;
	}
	jsonObj_->m_strKey = strKey;
	if (m_iType == TypeObject)
	{
		CopyJson(&jsonObj, jsonObj_);
		jsonObj_->m_strKey = strKey;
		if (bNeedPush)
			m_LstVal->push_back(jsonObj_);
		return true;
	}
	else if (m_iType == TypeNull)
	{
		CopyJson(&jsonObj, jsonObj_);
		jsonObj_->m_strKey = strKey;
		m_LstVal = new std::vector<VcdstarJson*>();
		m_LstVal->push_back(jsonObj_);
		m_iType = TypeObject;
		return true;
	}
	else
	{
		delete jsonObj_;
	}
	return false;
}

void VcdstarJson::ReleaseThis()
{
	if (m_iType == TypeString)
	{
		delete m_strVal;
	}
	else if (m_iType == TypeArray || m_iType == TypeObject)
	{
		for (std::vector<VcdstarJson*>::iterator it = m_LstVal->begin()
			; it != m_LstVal->end()
			; it++)
		{
			delete (*it);
		}
		m_LstVal->clear();
		delete m_LstVal;
	}
	m_iType = TypeNull;
	m_strKey = "";
}

bool VcdstarJson::ParseJson_(const std::string& strJson)
{
	ReleaseThis();
	int iIndex = 0;
	if (iIndex >= strJson.size() || (iIndex = JumpSpace(strJson, iIndex)) == -1)
		return false;

	if (strJson[iIndex] == '{')
	{
		m_iType = TypeObject;
		m_LstVal = new std::vector<VcdstarJson*>();
		if (ParseObject(strJson, iIndex, m_LstVal) == -1) {
			ReleaseThis();
			return false;
		}
	}
	else if (strJson[iIndex] == '[')
	{
		m_iType = TypeObject;
		m_LstVal = new std::vector<VcdstarJson*>();
		if (ParseArray(strJson, iIndex, m_LstVal) == -1) {
			ReleaseThis();
			return false;
		}
	}
	else
	{
		ReleaseThis();
		return false;
	}
	return true;
}

int VcdstarJson::ParseObject(const std::string& strJson, int iIndex, std::vector<VcdstarJson*>* obj)
{
	int iRet = iIndex;
	if ((iRet = JumpSpace(strJson, ++iRet)) == -1)
		return -1;
	while (strJson[iRet] != '}')
	{
		VcdstarJson* jsonObj = new VcdstarJson();
		obj->push_back(jsonObj);
		if ((iRet = ParseString(strJson, iRet, &jsonObj->m_strKey)) == -1
			|| (iRet = JumpSpace(strJson, ++iRet)) == -1
			|| strJson[iRet] != ':'
			|| (iRet = JumpSpace(strJson, ++iRet)) == -1
			|| (iRet = ParseValue(strJson, iRet, jsonObj)) == -1
			|| (iRet = JumpSpace(strJson, ++iRet)) == -1)
			return -1;

		if (strJson[iRet] == '}')
			return iRet;
		else if (strJson[iRet] == ',')
		{
			if ((iRet = JumpSpace(strJson, ++iRet)) == -1 || strJson[iRet] == '}')
				return -1;
		}
		else
			return -1;
	}
	return iRet;
}

int VcdstarJson::ParseArray(const std::string& strJson, int iIndex, std::vector<VcdstarJson*>* obj)
{
	int iRet = iIndex;
	if ((iRet = JumpSpace(strJson, ++iRet)) == -1)
		return -1;
	while (strJson[iRet] != ']')
	{
		VcdstarJson* jsonObj = new VcdstarJson();
		obj->push_back(jsonObj);
		if ((iRet = ParseValue(strJson, iRet, jsonObj)) == -1
			|| (iRet = JumpSpace(strJson, ++iRet)) == -1)
			return -1;

		if (strJson[iRet] == ']')
			return iRet;
		else if (strJson[iRet] == ',')
		{
			if ((iRet = JumpSpace(strJson, ++iRet)) == -1 || strJson[iRet] == ']')
				return -1;
		}
		else
			return -1;
	}
	return iRet;
}

int VcdstarJson::ParseValue(const std::string& strJson, int iIndex, VcdstarJson* jsonObj)
{
	int iRet = iIndex;
	if (strJson[iRet] == '{')
	{
		jsonObj->m_iType = TypeObject;
		jsonObj->m_LstVal = new std::vector<VcdstarJson*>();
		iRet = ParseObject(strJson, iRet, jsonObj->m_LstVal);
	}
	else if (strJson[iRet] == '"')
	{
		jsonObj->m_iType = TypeString;
		jsonObj->m_strVal = new std::string();
		iRet = ParseString(strJson, iRet, jsonObj->m_strVal);
	}
	else if (strJson[iRet] == '[')
	{
		jsonObj->m_iType = TypeArray;
		jsonObj->m_LstVal = new std::vector<VcdstarJson*>();
		iRet = ParseArray(strJson, iRet, jsonObj->m_LstVal);
	}
	else if (iRet + 3 < strJson.size() && strJson[iRet] == 'n' && strJson[iRet + 1] == 'u' && strJson[iRet + 2] == 'l' && strJson[iRet + 3] == 'l')
	{
		jsonObj->m_iType = TypeNull;
		iRet += 3;
	}
	else if (iRet + 3 < strJson.size() && strJson[iRet] == 't' && strJson[iRet + 1] == 'r' && strJson[iRet + 2] == 'u' && strJson[iRet + 3] == 'e')
	{
		jsonObj->m_iType = TypeBool;
		jsonObj->m_bVal = true;
		iRet += 3;
	}
	else if (iRet + 4 < strJson.size() && strJson[iRet] == 'f' && strJson[iRet + 1] == 'a' && strJson[iRet + 2] == 'l' && strJson[iRet + 3] == 's' && strJson[iRet + 4] == 'e')
	{
		jsonObj->m_iType = TypeBool;
		jsonObj->m_bVal = false;
		iRet += 4;
	}
	else if ((strJson[iRet] >= '0' && strJson[iRet] <= '9') || strJson[iRet] == '-')
	{
		iRet = ParseNumber(strJson, iRet, jsonObj);
	}
	else
	{
		return -1;
	}
	return iRet;
}

int VcdstarJson::ParseString(const std::string& strJson, int iIndex, std::string* jsonStr)
{
	int iRet = iIndex;
	if (strJson[iRet] != '"')
		return -1;
	else
		++iRet;
	while (strJson[iRet] != '"')
	{
		if (strJson[iRet] == '\\')
		{ 
			if (++iRet >= strJson.size())
				return -1;
			if (strJson[iRet] == '\\')
				jsonStr->append("\\");
			else if (strJson[iRet] == 'b')
				(*jsonStr) += '\b';
			else if (strJson[iRet] == 'f')
				(*jsonStr) += '\f';
			else if (strJson[iRet] == 't')
				(*jsonStr) += '\t';
			else if (strJson[iRet] == 'n')
				(*jsonStr) += '\n';
			else if (strJson[iRet] == 'r')
				(*jsonStr) += '\r';
			else if (strJson[iRet] == '"')
				(*jsonStr) += '\"';
			else {
				(*jsonStr) += '\\';
				(*jsonStr) += strJson[iRet];
			}
		}
		else
		{
			(*jsonStr) += strJson[iRet];
		}
		if (++iRet >= strJson.size())
			return -1;
	}
	return iRet;
}

int VcdstarJson::ParseNumber(const std::string& strJson, int iIndex, VcdstarJson* jsonObj)
{
	int iRet = iIndex;
	std::string strNum;
	if (strJson[iRet] == '-')
	{
		strNum.append("-");
		iRet++;
	}

	bool bDouble = false;
	while ((strJson[iRet] >= '0' && strJson[iRet] <= '9') || (!bDouble && strJson[iRet] == '.'))
	{
		strNum += strJson[iRet];
		if (strJson[iRet] == '.' && !bDouble)
			bDouble = true;
		iRet++;
	}
	if (bDouble)
	{
		jsonObj->m_iType = TypeDouble;
		jsonObj->m_dVal = atof(strNum.c_str());
	}
	else
	{
		jsonObj->m_iType = TypeInt;
		jsonObj->m_iVal = atoll(strNum.c_str());
	}
	return --iRet;
}

int VcdstarJson::JumpSpace(const std::string& strJson, int iIndex)
{
	int iRet = iIndex;
	if (iRet >= strJson.size())
		return -1;
	while (strJson[iRet] == ' '
		|| strJson[iRet] == '\n'
		|| strJson[iRet] == '\0'
		|| strJson[iRet] == '\r'
		|| strJson[iRet] == '\t')
	{
		if (++iRet >= strJson.size())
			return -1;
	}
	return iRet;
}

std::string VcdstarJson::JosnObjectToString(std::vector<VcdstarJson*>* obj, int deep, std::string prefix, std::string indent)
{
	std::string strRet;
	strRet.append("{");
	strRet.append(prefix);
	std::string indent_;
	for (int i = 0; i < deep; ++i)
		indent_.append(indent);
	for (std::vector<VcdstarJson*>::iterator it = obj->begin(); it != obj->end(); it++)
	{
		if (it != obj->begin()) {
			strRet.append(",");
			strRet.append(prefix);
		}
		strRet.append(indent_);
		strRet.append("\"");
		strRet.append((*it)->m_strKey);
		strRet.append("\"");
		strRet.append(": ");
		if ((*it)->m_iType == TypeInt)
		{
			char szTmp[64] = { 0 };
			sprintf(szTmp, "%lld", (*it)->m_iVal);
			strRet.append(szTmp);
		}
		else if ((*it)->m_iType == TypeDouble)
		{
			char szTmp[32] = { 0 };
			sprintf(szTmp, "%.13f", (*it)->m_dVal);
			strRet.append(szTmp);
		}
		else if ((*it)->m_iType == TypeNull)
		{
			strRet.append("null");
		}
		else if ((*it)->m_iType == TypeString)
		{
			strRet.append("\"");
			strRet.append(ToJsonString(*((*it)->m_strVal)));
			strRet.append("\"");
		}
		else if ((*it)->m_iType == TypeBool)
		{
			if ((*it)->m_bVal)
				strRet.append("true");
			else
				strRet.append("false");
		}
		else if ((*it)->m_iType == TypeArray)
		{
			strRet.append(JosnArrayToString((*it)->m_LstVal, deep + 1, prefix, indent));
		}
		else if ((*it)->m_iType == TypeObject)
		{
			strRet.append(JosnObjectToString((*it)->m_LstVal, deep + 1, prefix, indent));
		}
	}

	strRet.append(prefix);
	if (indent.size() > 0)
		for (int i = 0; i < deep - 1; ++i)
			strRet.append(indent);
	strRet.append("}");

	return strRet;
}

std::string VcdstarJson::JosnArrayToString(std::vector<VcdstarJson*>* obj, int deep, std::string prefix, std::string indent)
{
	std::string strRet;
	strRet.append("[");
	strRet.append(prefix);
	std::string indent_;
	for (int i = 0; i < deep; ++i)
		indent_.append(indent);
	for (std::vector<VcdstarJson*>::iterator it = obj->begin(); it != obj->end(); it++)
	{
		if (it != obj->begin()) {
			strRet.append(",");
			strRet.append(prefix);
		}
		strRet.append(indent_);
		if ((*it)->m_iType == TypeInt)
		{
			char szTmp[64] = { 0 };
			sprintf(szTmp, "%lld", (*it)->m_iVal);
			strRet.append(szTmp);
		}
		else if ((*it)->m_iType == TypeDouble)
		{
			char szTmp[32] = { 0 };
			sprintf(szTmp, "%.13f", (*it)->m_dVal);
			strRet.append(szTmp);
		}
		else if ((*it)->m_iType == TypeNull)
		{
			strRet.append("null");
		}
		else if ((*it)->m_iType == TypeString)
		{
			strRet.append("\"");
			strRet.append(ToJsonString(*((*it)->m_strVal)));
			strRet.append("\"");
		}
		else if ((*it)->m_iType == TypeBool)
		{
			if ((*it)->m_bVal)
				strRet.append("true");
			else
				strRet.append("false");
		}
		else if ((*it)->m_iType == TypeArray)
		{
			strRet.append(JosnArrayToString((*it)->m_LstVal, deep + 1, prefix, indent));
		}
		else if ((*it)->m_iType == TypeObject)
		{
			strRet.append(JosnObjectToString((*it)->m_LstVal, deep + 1, prefix, indent));
		}
	}

	strRet.append(prefix);
	if (indent.size() > 0)
		for (int i = 0; i < deep - 1; ++i)
			strRet.append(indent);
	strRet.append("]");

	return strRet;
}

std::string VcdstarJson::ToJsonString(const std::string& src)
{
	std::string strRet;
	for (int i = 0; i < src.size(); ++i)
	{
		if (src[i] == '\n')
			strRet.append("\\n");
		else if (src[i] == '\t')
			strRet.append("\\t");
		else if (src[i] == '\r')
			strRet.append("\\r");
		else if (src[i] == '\f')
			strRet.append("\\f");
		else if (src[i] == '\b')
			strRet.append("\\b");
		else if (src[i] == '\\')
			strRet.append("\\\\");
		else if (src[i] == '"')
			strRet.append("\\\"");
		else
			strRet += src[i];
	}
	return strRet;
}

bool VcdstarJson::AddJsonNode(VcdstarJson* jsonObj, int iType, bool bNeedPush)
{
	if (m_iType == iType)
	{
		if (bNeedPush)
			m_LstVal->push_back(jsonObj);
		return true;
	}
	else if (m_iType == TypeNull)
	{
		m_LstVal = new std::vector<VcdstarJson*>();
		m_LstVal->push_back(jsonObj);
		m_iType = iType;
		return true;
	}
	else
	{
		delete jsonObj;
	}
	return false;
}

bool VcdstarJson::InsertJsonNode(const int& Position, VcdstarJson* jsonObj)
{
	if (m_iType != TypeArray || Position < 0 || Position >= m_LstVal->size())
	{
		delete jsonObj;
		return false;
	}
	m_LstVal->insert(m_LstVal->begin() + Position, jsonObj);
	return true;
}

VcdstarJson* VcdstarJson::CheckSame(const std::string& key)
{
	if (m_iType == TypeObject)
		for (std::vector<VcdstarJson*>::iterator it = m_LstVal->begin()
			; it != m_LstVal->end(); it++)
		{
			if ((*it)->m_strKey == std::string(key))
			{
				(*it)->ReleaseThis();
				return *it;
			}
		}
	return nullptr;
}

void VcdstarJson::CopyJson(const VcdstarJson* const src, VcdstarJson* dst)
{
	dst->m_strKey = src->m_strKey;
	dst->m_iType = src->m_iType;
	if (src->m_iType == TypeInt)
	{
		dst->m_iVal = src->m_iVal;
	}
	else if (src->m_iType == TypeDouble)
	{
		dst->m_dVal = src->m_dVal;
	}
	else if (src->m_iType == TypeString)
	{
		dst->m_strVal = new std::string(*(src->m_strVal));
	}
	else if (src->m_iType == TypeBool)
	{
		dst->m_bVal = src->m_bVal;
	}
	else if (src->m_iType == TypeArray || src->m_iType == TypeObject)
	{
		dst->m_LstVal = new std::vector<VcdstarJson*>();
		for (std::vector<VcdstarJson*>::iterator it = src->m_LstVal->begin()
			; it != src->m_LstVal->end(); it++)
		{
			VcdstarJson* jsonObj = new VcdstarJson();
			CopyJson(*it, jsonObj);
			dst->m_LstVal->push_back(jsonObj);
		}
	}
}