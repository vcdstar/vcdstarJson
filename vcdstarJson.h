#pragma once
#include <string>
#include <iostream>
#include <queue>
using namespace std;

namespace vcdstarJson
{
	// Json �ڵ������
	enum NodeType
	{
		type_int = 0,
		type_double,
		type_string,
		type_array,
		type_object,
		type_bool,
		type_null,
		type_undefined
	};
	
	// Json �ڵ�
	typedef struct JsonNode_
	{
		NodeType _type;// ����
		string _key;// key��string
		double d_val;// �����Ͷ���
		long long i_val;// ���ζ���
		string str_val;// string����
		bool b_val;// bool����
		JsonNode_* obj_val;// json����ڵ�
		int arrLen;// ����ĳ���
		JsonNode_* arr_val;// json�������Ľڵ�

		JsonNode_* next;// ��һ���ڵ�
		JsonNode_* prev;// ��һ���ڵ�
		JsonNode_* father;// ���׽ڵ�
		JsonNode_()
		{
			_type = NodeType::type_undefined;
			d_val = 0.0;
			i_val = 0;
			b_val = false;
			obj_val = nullptr;
			arrLen = 0;
			arr_val = nullptr;

			next = nullptr;
			prev = nullptr;
			father = nullptr;
		}
	}JsonNode;

	// ����json node ��������
	int release_node(JsonNode*& node_) {
		queue<JsonNode*> q;
		q.push(node_);
		while (q.size() > 0)
		{
			JsonNode* node = q.front();
			if (node != nullptr && node->_type == NodeType::type_object)
			{
				q.push(node->obj_val);
				node->obj_val = nullptr;
			}
			if (node != nullptr && node->_type == NodeType::type_array)
			{
				q.push(node->arr_val);
				node->arr_val = nullptr;
			}
			if (node != nullptr && node->next != nullptr)
			{
				q.push(node->next);
				node->next = nullptr;
			}
			delete node;
			node = nullptr;
			q.pop();
		}
		node_ = nullptr;
		return 0;
	}

	// ��node��ʼ���ˣ��ָ�ֵΪĬ��
	int init_node_val(JsonNode*& node_) {
		node_->_type = NodeType::type_undefined;
		node_->_key.clear();
		node_->d_val = 0.0;
		node_->i_val = 0;
		node_->str_val.clear();
		node_->b_val = false;
		release_node(node_->obj_val);
		node_->arrLen = 0;
		release_node(node_->arr_val);
		return 0;
	}

	// ����node��ֵ������/ָ�벻����
	int copy_node(JsonNode*& src, JsonNode*& dst) {
		if (src == nullptr || dst == nullptr)
			return -1;
		dst->_type = src->_type;
		dst->_key = src->_key;
		dst->d_val = src->d_val;
		dst->i_val = src->i_val;
		dst->str_val = src->str_val;
		dst->b_val = src->b_val;
		dst->arrLen = src->arrLen;
		return 0;
	}

	// ���node��ֵ
	int deep_copy_node(JsonNode*& src, JsonNode*& dst, JsonNode* father, JsonNode* prev) {
		if (src == nullptr || dst != nullptr)// ����Ϊ�գ���ͷ����������ݹ�
			return -1;
		dst = new JsonNode();
		dst->_type = src->_type;
		if (src->_type == NodeType::type_object) {
			deep_copy_node(src->obj_val, dst->obj_val, src, nullptr);
		}
		else if (src->_type == NodeType::type_array) {
			deep_copy_node(src->arr_val, dst->arr_val, src, nullptr);
		}
		dst->_key = src->_key;
		dst->d_val = src->d_val;
		dst->i_val = src->i_val;
		dst->str_val = src->str_val;
		dst->b_val = src->b_val;
		dst->arrLen = src->arrLen;
		dst->father = father;
		dst->prev = prev;
		if (src->next != nullptr) {
			deep_copy_node(src->next, dst->next, father, dst);
		}
		return 0;
	}

	// ��Ҫ��json����
	class Json
	{
	public:
		// ���췽����Ĭ��
		Json() : root_node(nullptr), m_bDelete(false), m_iArrLen(-1), m_nodeType(NodeType::type_undefined) {

		}
		
		// ���췽������string�������ɶ���
		Json(string& strJson) : m_bDelete(true), m_iArrLen(-1), m_nodeType(NodeType::type_undefined)
		{
			root_node = nullptr;
			for (int i = 0; i < strJson.size(); i++)
			{
				if (strJson[i] == ' ' || strJson[i] == '\n' || strJson[i] == '\r')
					continue;
				else if (strJson[i] == '{') {
					root_node = new JsonNode();
					m_nodeType = NodeType::type_object;
					if (parse_obj(root_node, strJson, i) == -1)
						release_node(root_node);
					break;
				}
				else if (strJson[i] == '[') {
					root_node = new JsonNode();
					m_nodeType = NodeType::type_array;
					root_node->_type = NodeType::type_array;
					if (parse_arr(root_node, strJson, i, ++m_iArrLen) == -1)
						release_node(root_node);
					break;
				}
				else {
					break;
				}
			}
		}

		// �������캯������ֹ�������Ķ���delete
		Json(const Json& j) {
			m_bDelete = false;// �Ƿ���Ҫɾ��
			m_iArrLen = j.m_iArrLen;// ���鳤��
			m_nodeType = j.m_nodeType;// �ڵ�����
			root_node = j.root_node;// ���ڵ�
		}

		// ���������������Ҫɾ�����ڵ�
		~Json() {
			if (m_bDelete) {
				release_node(root_node);
				cout << "json release \n";
			}
		}
	
		// ����[string]��Ѱ�Ҷ���
		Json operator[](string _key) {
			if (root_node != nullptr && m_nodeType != NodeType::type_object)
				return Json(nullptr, false, -1, NodeType::type_undefined);
			JsonNode* next_ = root_node;// ���������Ľڵ�
			JsonNode* first_undefined = nullptr;// ��¼��һ��δʹ�õĽڵ㣬�����½��ڵ�
			JsonNode* last = nullptr;// ��¼���һ�����ýڵ㣬�����½��ڵ�
			while (next_){
				if (next_->_key == _key) {
					if (next_->_type == NodeType::type_object)
						return Json(next_->obj_val, false, -1, NodeType::type_object);
					else if (next_->_type == NodeType::type_array)
						return Json(next_->arr_val, false, next_->arrLen, NodeType::type_array);
					else
						return Json(next_, false, -1, next_->_type);
				}
				if (next_->_type == NodeType::type_undefined && first_undefined == nullptr)
					first_undefined = next_;
				if (next_->next == nullptr) {
					last = next_;
				}
				next_ = next_->next;
			}
			if (first_undefined != nullptr) {
				first_undefined->_key = _key;
				first_undefined->_type = NodeType::type_null;
				return Json(first_undefined, false, -1, NodeType::type_null);
			}
			else if (last != nullptr){
				last->next = new JsonNode();
				last->next->prev = last;
				last->next->father = last->father;
				last->next->_key = _key;
				last->next->_type = NodeType::type_null;
				return Json(last->next, false, -1, NodeType::type_null);
			}
			else {
				m_bDelete = true;
				root_node = new JsonNode();
				m_nodeType = NodeType::type_object;
				root_node->_key = _key;
				root_node->_type = NodeType::type_null;
				return Json(root_node, false, -1, NodeType::type_null);
			}
		}

		// ����[int]��Ѱ�Ҷ���
		Json operator[](int _index) {
			if (root_node == nullptr)// ���鲻��ͨ��[]�ķ�ʽ�����������������û�д���ֱ�ӷ��ؿ�
				return Json(nullptr, false, -1, NodeType::type_undefined);
			if (m_nodeType != NodeType::type_array || m_iArrLen <= -1)// ��������������
				return Json(nullptr, false, -1, NodeType::type_undefined);
			if (_index < 0 || _index >= m_iArrLen)// ��ֹ����Խ��
				return Json(nullptr, false, -1, NodeType::type_undefined);

			JsonNode* next_ = root_node;// ���������Ľڵ�
			int iIndex = -1;// ����
			while (++iIndex != _index) {
				next_ = next_->next;
			}
			if (next_->_type == NodeType::type_object)
				return Json(next_->obj_val, false, -1, NodeType::type_object);
			else if (next_->_type == NodeType::type_array)
				return Json(next_->arr_val, false, next_->arrLen, NodeType::type_array);
			else
				return Json(next_, false, -1, next_->_type);
		}

		// object����=int
		void operator=(const int _val) {
			long long ll_val = _val;
			operator=(ll_val);
		}

		// object����=short
		void operator=(const short _val) {
			long long ll_val = _val;
			operator=(ll_val);
		}

		// object����=long
		void operator=(const long _val) {
			long long ll_val = _val;
			operator=(ll_val);
		}

		// object����=longlong
		void operator=(const long long _val) {
			if (root_node != nullptr) {
				if (m_nodeType == NodeType::type_object || m_nodeType == NodeType::type_array) {
					if (root_node->father == nullptr)// ��ǰ�Ǹ��ڵ���󣬲������޸�
						return;
					root_node = root_node->father;
				}
				string _key = root_node->_key;
				init_node_val(root_node);
				root_node->_type = NodeType::type_int;
				root_node->i_val = _val;
				root_node->_key = _key;
			}
		}

		// object����=float
		void operator=(const float _val) {
			double d_val = _val;
			operator=(d_val);
		}

		// object����=double
		void operator=(const double _val) {
			if (root_node != nullptr) {
				if (m_nodeType == NodeType::type_object || m_nodeType == NodeType::type_array) {
					if (root_node->father == nullptr)// ��ǰ�Ǹ��ڵ���󣬲������޸�
						return;
					root_node = root_node->father;
				}
				string _key = root_node->_key;
				init_node_val(root_node);
				root_node->_type = NodeType::type_double;
				root_node->d_val = _val;
				root_node->_key = _key;
			}
		}

		// object����=string
		void operator=(const string _val) {
			if (root_node != nullptr) {
				if (m_nodeType == NodeType::type_object || m_nodeType == NodeType::type_array) {
					if (root_node->father == nullptr)// ��ǰ�Ǹ��ڵ���󣬲������޸�
						return;
					root_node = root_node->father;
				}
				string _key = root_node->_key;
				init_node_val(root_node);
				root_node->_type = NodeType::type_string;
				root_node->str_val = _val;
				root_node->_key = _key;
			}
		}

		// object����=bool
		void operator=(const bool _val) {
			if (root_node != nullptr) {
				if (m_nodeType == NodeType::type_object || m_nodeType == NodeType::type_array) {
					if (root_node->father == nullptr)// ��ǰ�Ǹ��ڵ���󣬲������޸�
						return;
					root_node = root_node->father;
				}
				string _key = root_node->_key;
				init_node_val(root_node);
				root_node->_type = NodeType::type_bool;
				root_node->b_val = _val;
				root_node->_key = _key;
			}
		}

		// object����=null
		void operator=(const void* _val) {
			if (root_node != nullptr) {
				if (m_nodeType == NodeType::type_object || m_nodeType == NodeType::type_array) {
					if (root_node->father == nullptr)// ��ǰ�Ǹ��ڵ���󣬲������޸�
						return;
					root_node = root_node->father;
				}
				string _key = root_node->_key;
				init_node_val(root_node);
				root_node->_type = NodeType::type_null;
				root_node->_key = _key;
			}
		}

		// object����=json
		void operator=(Json _val) {
			_val.m_bDelete = false;// ��ֹ�����˳����������Ѵ������Ľڵ�ֵ��ɾ����
			if (root_node == nullptr && _val.root_node != nullptr && (_val.m_nodeType == NodeType::type_array || _val.m_nodeType == NodeType::type_object))// ���ǵ�ֱ�Ӹ�ֵ�����
			{
				deep_copy_node(_val.root_node, root_node, root_node, nullptr);
				m_bDelete = true;
				m_iArrLen = _val.m_iArrLen;
				m_nodeType = _val.m_nodeType;
				return;
			}
			if (root_node != nullptr && _val.root_node != nullptr && (_val.m_nodeType == NodeType::type_array || _val.m_nodeType == NodeType::type_object)) {
				if (m_nodeType == NodeType::type_object || m_nodeType == NodeType::type_array) {
					if (root_node->father == nullptr)// ��ǰ�Ǹ��ڵ���󣬲������޸�
						return;
					root_node = root_node->father;
				}
				JsonNode* node_tmp = nullptr;
				deep_copy_node(_val.root_node, node_tmp, root_node, nullptr);
				string _key = root_node->_key;
				init_node_val(root_node);
				root_node->_key = _key;
				if (_val.m_nodeType == NodeType::type_array) {
					root_node->_type = NodeType::type_array;
					root_node->arrLen = _val.m_iArrLen;
					root_node->arr_val = node_tmp;
				}
				else if (_val.m_nodeType == NodeType::type_object) {
					root_node->_type = NodeType::type_object;
					root_node->obj_val = node_tmp;
				}
			}
		}

		// �����յ�object����
		bool createEmptyObject() {
			if (root_node != nullptr)
				return false;

			root_node = new JsonNode();
			m_nodeType = NodeType::type_object;
			m_bDelete = true;
		}

		// �����յ�array����
		bool createEmptyArray() {
			if (root_node != nullptr)
				return false;

			root_node = new JsonNode();
			m_nodeType = NodeType::type_array;
			m_bDelete = true;
			m_iArrLen = 0;
		}

		// ------����������ķ���--------
		// �������int����
		bool add(const int _val) {
			long long l_val = _val;
			return add(l_val);
		}

		// �������short����
		bool add(const short _val) {
			long long l_val = _val;
			return add(l_val);
		}

		// �������long����
		bool add(const long _val) {
			long long l_val = _val;
			return add(l_val);
		}

		// �������longlong����
		bool add(const long long _val) {
			JsonNode* node = find_arr_add_node();
			if (node == nullptr)
				return false;
			node->_type = NodeType::type_int;
			node->i_val = _val;
			return true;
		}

		// �������float����
		bool add(const float _val) {
			double d_val = _val;
			return add(d_val);
		}

		// �������double����
		bool add(const double _val) {
			JsonNode* node = find_arr_add_node();
			if (node == nullptr)
				return false;
			node->_type = NodeType::type_double;
			node->d_val = _val;
			return true;
		}

		// �������string����
		bool add(const string _val) {
			JsonNode* node = find_arr_add_node();
			if (node == nullptr)
				return false;
			node->_type = NodeType::type_string;
			node->str_val = _val;
			return true;
		}

		// �������bool����
		bool add(const bool _val) {
			JsonNode* node = find_arr_add_node();
			if (node == nullptr)
				return false;
			node->_type = NodeType::type_bool;
			node->b_val = _val;
			return true;
		}

		// �������null����
		bool add() {
			JsonNode* node = find_arr_add_node();
			if (node == nullptr)
				return false;
			node->_type = NodeType::type_null;
			return true;
		}

		// �������json����
		bool add(Json _val) {
			_val.m_bDelete = false;// ��ֹ�����˳����������Ѵ������Ľڵ�ֵ��ɾ����
			if (_val.root_node == nullptr || (_val.m_nodeType != NodeType::type_array && _val.m_nodeType != NodeType::type_object))// Ҫ���ӵĽڵ㲻��Ϊ��,Ҳ����Ϊ������߶�������Ľڵ�
				return false;
			JsonNode* node = find_arr_add_node();
			if (node == nullptr)
				return false;

			JsonNode* node_tmp = nullptr;
			deep_copy_node(_val.root_node, node_tmp, root_node, nullptr);
			if (_val.m_nodeType == NodeType::type_array) {
				node->_type = NodeType::type_array;
				node->arrLen = _val.m_iArrLen;
				node->arr_val = node_tmp;
			}
			else if (_val.m_nodeType == NodeType::type_object) {
				node->_type = NodeType::type_object;
				node->obj_val = node_tmp;
			}

			return true;
		}

		// ����insert null����
		bool insert(int _index) {
			JsonNode* node = find_arr_insert_node(_index);
			if (node == nullptr)
				return false;

			node->_type = NodeType::type_null;
			return true;
		}

		// ����insert int����
		bool insert(int _index, const int _val) {
			long long l_val = _val;
			return insert(_index, l_val);
		}

		// ����insert short����
		bool insert(int _index, const short _val) {
			long long l_val = _val;
			return insert(_index, l_val);
		}

		// ����insert long����
		bool insert(int _index, const long _val) {
			long long l_val = _val;
			return insert(_index, l_val);
		}

		// ����insert longlong����
		bool insert(int _index, const long long _val) {
			JsonNode* node = find_arr_insert_node(_index);
			if (node == nullptr)
				return false;

			node->_type = NodeType::type_int;
			node->i_val = _val;
			return true;
		}

		// ����insert float����
		bool insert(int _index, const float _val) {
			double d_val = _val;
			return insert(_index, d_val);
		}

		// ����insert double����
		bool insert(int _index, const double _val) {
			JsonNode* node = find_arr_insert_node(_index);
			if (node == nullptr)
				return false;

			node->_type = NodeType::type_double;
			node->d_val = _val;
			return true;
		}

		// ����insert bool����
		bool insert(int _index, const bool _val) {
			JsonNode* node = find_arr_insert_node(_index);
			if (node == nullptr)
				return false;

			node->_type = NodeType::type_bool;
			node->b_val = _val;
			return true;
		}

		// ����insert string����
		bool insert(int _index, const string _val) {
			JsonNode* node = find_arr_insert_node(_index);
			if (node == nullptr)
				return false;

			node->_type = NodeType::type_string;
			node->str_val = _val;
			return true;
		}
		
		// ����insert json����
		bool insert(int _index, Json _val) {
			_val.m_bDelete = false;
			if (_val.m_nodeType != NodeType::type_array && _val.m_nodeType != NodeType::type_object)
				return false;

			JsonNode* node_tmp = nullptr;
			deep_copy_node(_val.root_node, node_tmp, root_node, nullptr);

			JsonNode* node = find_arr_insert_node(_index);
			if (node == nullptr) {
				release_node(node_tmp);
				return false;
			}

			if (_val.m_nodeType == NodeType::type_array) {
				node->arr_val = node_tmp;
				node->_type = NodeType::type_array;
				node->arrLen = _val.m_iArrLen;
			}
			else {
				node->obj_val = node_tmp;
				node->_type = NodeType::type_object;
			}

			return true;
		}

		// ת��string����
		string toString() {
			string ret;
			if (root_node == nullptr)
				return ret;
			if (m_nodeType == NodeType::type_object) {
				obj_to_string(root_node, ret);
			}
			else if (m_nodeType == NodeType::type_array) {
				arr_to_string(root_node, ret);
			}
			return ret;
		}
	private:
		// ˽�й��췽��
		Json(JsonNode* _node, bool bDelete, int iArrLen, NodeType nodetype) {
			root_node = _node;
			m_bDelete = bDelete;
			m_iArrLen = iArrLen;
			m_nodeType = nodetype;
		}

		// ���������п���add�����Ľڵ�
		JsonNode* find_arr_add_node() {
			if (root_node == nullptr || m_nodeType != NodeType::type_array)
				return nullptr;
			JsonNode* next = root_node;// �����ҿ���add�Ķ���
			while (next != nullptr) {
				if (next->_type == NodeType::type_undefined)
				{
					m_iArrLen++;
					if (root_node->father != nullptr)
						root_node->father->arrLen = m_iArrLen;
					return next;
				}
				if (next->next == nullptr) {
					m_iArrLen++;
					if (root_node->father != nullptr)
						root_node->father->arrLen = m_iArrLen;
					next->next = new JsonNode();
					next->next->father = next->father;
					next->next->prev = next;
					return next->next;
				}
				next = next->next;
			}
			return nullptr;
		}

		// ���������п���insert�����Ľ��
		JsonNode* find_arr_insert_node(int _index) {
			if (root_node == nullptr || m_iArrLen < 0 || m_iArrLen <= _index || m_nodeType != NodeType::type_array)
				return nullptr;

			JsonNode* next = root_node;// �����ҿ���add�Ķ���
			int iIndex = -1;
			while (++iIndex != _index) {
				next = next->next;
			}
			m_iArrLen++;
			if (root_node->father != nullptr)
				root_node->father->arrLen++;

			JsonNode* new_node = new JsonNode();
			new_node->father = next->father;
			new_node->prev = next->prev;

			if (next->prev != nullptr) {
				next->prev->next = new_node;
			}
			new_node->next = next;
			next->prev = new_node;
			if (_index == 0)// ��ֹ����0��ʱ���Ҳ���ͷ�ڵ�
				root_node = new_node;

			return new_node;
		}

		// object ת string ����
		void obj_to_string(JsonNode* node_, string& str_json) {
			str_json += "{";
			JsonNode* node_tmp = node_;
			bool bFirst = true;
			while (node_tmp != nullptr) {
				if (node_tmp->_type != NodeType::type_undefined) {
					if (bFirst)
						bFirst = false;
					else
						str_json += ",";

					str_json += "\"";
					str_json += node_tmp->_key;
					if (node_tmp->_type == NodeType::type_null) {
						str_json += "\":null";
					}
					else if (node_tmp->_type == NodeType::type_bool) {
						if (node_tmp->b_val)
							str_json += "\":true";
						else
							str_json += "\":false";
					}
					else if (node_tmp->_type == NodeType::type_int) {
						str_json += "\":";
						str_json += std::to_string(node_tmp->i_val);
					}
					else if (node_tmp->_type == NodeType::type_double) {
						str_json += "\":";
						char szTmp[1024] = { 0 };
						sprintf(szTmp, "%0.13f", node_tmp->d_val);
						str_json += szTmp;//std::to_string(node_tmp->d_val);
					}
					else if (node_tmp->_type == NodeType::type_string) {
						str_json += "\":\"";
						// ����ת���ַ�
						for (int i = 0; i < node_tmp->str_val.size(); i++) {
							if (node_tmp->str_val[i] == '\/')
								str_json += "\\/";
							else if (node_tmp->str_val[i] == '\b')
								str_json += "\\b";
							else if (node_tmp->str_val[i] == '\f')
								str_json += "\\f";
							else if (node_tmp->str_val[i] == '\t')
								str_json += "\\t";
							else if (node_tmp->str_val[i] == '\n')
								str_json += "\\n";
							else if (node_tmp->str_val[i] == '\r')
								str_json += "\\r";
							else if (node_tmp->str_val[i] == '\\')
								str_json += "\\\\";
							else if (node_tmp->str_val[i] == '\"')
								str_json += "\\\"";
							else
								str_json += node_tmp->str_val[i];
						}
						str_json += "\"";
					}
					else if (node_tmp->_type == NodeType::type_object) {
						str_json += "\":";
						obj_to_string(node_tmp->obj_val, str_json);
					}
					else if (node_tmp->_type == NodeType::type_array) {
						str_json += "\":";
						arr_to_string(node_tmp->arr_val, str_json);
					}
				}
				node_tmp = node_tmp->next;
			}
			str_json += "}";
		}

		// array ת string ����
		void arr_to_string(JsonNode* node_, string& str_json) {
			str_json += "[";
			JsonNode* node_tmp = node_;
			bool bFirst = true;
			while (node_tmp != nullptr) {
				if (node_tmp->_type != NodeType::type_undefined) {
					if (bFirst)
						bFirst = false;
					else
						str_json += ",";

					if (node_tmp->_type == NodeType::type_null) {
						str_json += "null";
					}
					else if (node_tmp->_type == NodeType::type_bool) {
						if (node_tmp->b_val)
							str_json += "true";
						else
							str_json += "false";
					}
					else if (node_tmp->_type == NodeType::type_int) {
						str_json += std::to_string(node_tmp->i_val);
					}
					else if (node_tmp->_type == NodeType::type_double) {
						char szTmp[1024] = { 0 };
						sprintf(szTmp, "%0.13f", node_tmp->d_val);
						str_json += szTmp;//std::to_string(node_tmp->d_val);
					}
					else if (node_tmp->_type == NodeType::type_string) {
						str_json += "\"";
						// ����ת���ַ�
						for (int i = 0; i < node_tmp->str_val.size(); i++) {
							if (node_tmp->str_val[i] == '\/')
								str_json += "\\/";
							else if (node_tmp->str_val[i] == '\b')
								str_json += "\\b";
							else if (node_tmp->str_val[i] == '\f')
								str_json += "\\f";
							else if (node_tmp->str_val[i] == '\t')
								str_json += "\\t";
							else if (node_tmp->str_val[i] == '\n')
								str_json += "\\n";
							else if (node_tmp->str_val[i] == '\r')
								str_json += "\\r";
							else if (node_tmp->str_val[i] == '\\')
								str_json += "\\\\";
							else if (node_tmp->str_val[i] == '\"')
								str_json += "\\\"";
							else
								str_json += node_tmp->str_val[i];
						}
						str_json += "\"";
					}
					else if (node_tmp->_type == NodeType::type_object) {
						obj_to_string(node_tmp->obj_val, str_json);
					}
					else if (node_tmp->_type == NodeType::type_array) {
						arr_to_string(node_tmp->arr_val, str_json);
					}
				}
				node_tmp = node_tmp->next;
			}
			str_json += "]";
		}

		// ����json Object����
		int parse_obj(JsonNode*& node_, string& str_json, int& start_index)
		{
			string tmp;
			JsonNode* index_Node = node_;// ��ǰ�Ľڵ�
			for (int i = start_index + 1; i < str_json.size(); i++)
			{
				find_jump(str_json, i);
				// ��ֹ��һ���ն���
				if (i < str_json.size() && str_json[i] == '}') {
					start_index = i;
					return 0;
				}
				// ����key
				if (find_char(str_json, '\"', i))
				{
					tmp.clear();
					if (parse_string(str_json, tmp, i)) {
						JsonNode* prev = index_Node->prev;
						while (prev != nullptr)
						{
							if (prev->_key == tmp)
								return -1;
							prev = prev->prev;
						}
						index_Node->_key = tmp;
					}
					else {
						return -1;
					}
				}
				else {
					return -1;
				}

				// �����м�ķֺ�
				if (i < str_json.size() && find_char(str_json, ':', ++i))
					i++;
				else
					return -1;
				// ����value
				find_jump(str_json, i);
				if (parse_value(index_Node, str_json, i) == -1)
					return -1;
				// ��������
				find_jump(str_json, ++i);
				if (i < str_json.size() && str_json[i] == ',') {
				}
				else if (i < str_json.size() && str_json[i] == '}') {
					start_index = i;
					return 0;
				}
				else
					return -1;
			}
			return -1;
		}

		//����json�������
		int parse_arr(JsonNode*& node_, string& str_json, int& start_index, int& iArrLen)
		{
			JsonNode* index_Node = node_;// ��ǰ�Ľڵ�

			for (int i = start_index + 1; i < str_json.size(); i++)
			{
				find_jump(str_json, i);
				// ��ֹ��һ��������
				if (i < str_json.size() && str_json[i] == ']') {
					start_index = i;
					return 0;
				}
				find_jump(str_json, i);// �������пո�
				if (parse_value(index_Node, str_json, i) == -1)// ����ֵ
					return -1;

				++iArrLen;
				i++;
				find_jump(str_json, i);
				if (i < str_json.size() && str_json[i] == ',') {
				}
				else if (i < str_json.size() && str_json[i] == ']') {
					start_index = i;
					return 0;
				}
				else
					return -1;
			}

			return 0;
		}

		// ����value
		int parse_value(JsonNode*& index_Node, string& str_json, int& iIndex)
		{
			string tmp;
			if (iIndex < str_json.size() && str_json[iIndex] == '\"')// string ���͵�ֵ
			{
				if (parse_string(str_json, tmp, iIndex)) {
					index_Node->str_val = tmp;
					index_Node->_type = NodeType::type_string;
				}
				else {
					return -1;
				}
			}
			else if (iIndex < str_json.size() && str_json[iIndex] == '{')//��������
			{
				index_Node->obj_val = new JsonNode();
				index_Node->_type = NodeType::type_object;
				index_Node->obj_val->father = index_Node;
				if (parse_obj(index_Node->obj_val, str_json, iIndex))
					return -1;
			}
			else if (iIndex < str_json.size() && str_json[iIndex] >= '0' && str_json[iIndex] <= '9') // ��ֵ����
			{
				int iNumType = 0;
				parse_nunber(str_json, tmp, iIndex, iNumType);
				--iIndex;
				if (iNumType == 0) {
					index_Node->i_val = stoll(tmp);
					index_Node->_type = NodeType::type_int;
				}
				else if (iNumType == 1) {
					index_Node->d_val = stod(tmp);
					index_Node->_type = NodeType::type_double;
				}
				else
					return -1;
			}
			else if (iIndex + 3 < str_json.size() && str_json[iIndex] == 'n' &&
				str_json[iIndex + 1] == 'u' &&
				str_json[iIndex + 2] == 'l' &&
				str_json[iIndex + 3] == 'l') //null����
			{
				index_Node->_type = NodeType::type_null;
				iIndex += 3;
			}
			else if (iIndex + 4 < str_json.size() && str_json[iIndex] == 'f' &&
				str_json[iIndex + 1] == 'a' &&
				str_json[iIndex + 2] == 'l' &&
				str_json[iIndex + 3] == 's' &&
				str_json[iIndex + 4] == 'e') //false����
			{
				index_Node->_type = NodeType::type_bool;
				index_Node->b_val = false;
				iIndex += 4;
			}
			else if (iIndex + 3 < str_json.size() && str_json[iIndex] == 't' &&
				str_json[iIndex + 1] == 'r' &&
				str_json[iIndex + 2] == 'u' &&
				str_json[iIndex + 3] == 'e') //true����
			{
				index_Node->_type = NodeType::type_bool;
				index_Node->b_val = true;
				iIndex += 3;
			}
			else if (iIndex < str_json.size() && str_json[iIndex] == '[')// ��������
			{
				index_Node->arr_val = new JsonNode();
				index_Node->_type = NodeType::type_array;
				index_Node->arr_val->father = index_Node;
				int iRet = parse_arr(index_Node->arr_val, str_json, iIndex, index_Node->arrLen);
				if (iRet == -1)
					return -1;
			}
			else
				return -1;

			index_Node->next = new JsonNode();
			index_Node->next->prev = index_Node;
			index_Node->next->father = index_Node->father;
			index_Node = index_Node->next;

			return 0;
		}

		// �����ո�ͻس���ֱ�����ҵ�ָ���ַ�
		bool find_char(string& str_json, char szChar, int& iIndex)
		{
			for (; iIndex < str_json.size(); iIndex++)
			{
				if (str_json[iIndex] == ' ' || str_json[iIndex] == '\n' || str_json[iIndex] == '\r')//�����ո�ͻس�
					continue;
				else if (str_json[iIndex] == szChar)
					return true;
				else
					return false;
			}
			return false;
		}

		// �����ո�ͻس�
		void find_jump(string& str_json, int& iIndex)
		{
			for (; iIndex < str_json.size(); iIndex++)
			{
				if (str_json[iIndex] == ' ' || str_json[iIndex] == '\n' || str_json[iIndex] == '\r')//�����ո�ͻس�
					continue;
				else
					return;
			}
		}

		//����string��Ҳ���Դ���key,����iIndex�����ǵ�һ��"��λ��
		bool parse_string(string& str_json, string& str_, int& iIndex)
		{
			for (int i = iIndex; i < str_json.size(); i++)
			{
				if (str_json[i] == '\"') {
					for (int j = i + 1; j < str_json.size(); j++) // ��������key
					{
						if (str_json[j] == '\\' && j + 1 < str_json.size())//����ת���ַ�
						{
							if (!parse_escape(str_json, str_, j))
								return false;
							j++;
						}
						else if (str_json[j] == '\"') {
							iIndex = j;
							return true;
						}
						else if (str_json[j] == '\b' ||
							str_json[j] == '\f' ||
							str_json[j] == '\t' ||
							str_json[j] == '\n' ||
							str_json[j] == '\r' ||
							str_json[j] == '\\' ||
							str_json[j] == '\"') {
							return false;
						}
						else {
							if (j + 1 >= str_json.size())
								return false;
							str_ += str_json[j];
						}
					}
				}
				else {
					return false;
				}
			}
			return false;
		}

		// ������ֵ���ͣ�������ֵ���͵�string
		void parse_nunber(string& str_json, string& str_, int& iIndex, int& iNumType)
		{
			for (; iIndex < str_json.size(); iIndex++)
			{
				if (str_json[iIndex] >= '0' && str_json[iIndex] <= '9')
					str_ += str_json[iIndex];
				else if (str_json[iIndex] == '.') {
					str_ += str_json[iIndex];
					iNumType++;
				}
				else
					return;
			}
		}

		//����ת���ַ�
		bool parse_escape(string& str_json, string& str_, int iIndex)
		{
			if (iIndex + 1 >= str_json.size())
				return false;
			switch (str_json[iIndex + 1])
			{
			case 'b':
				str_ += "\b";
				break;
			case 'f':
				str_ += "\f";
				break;
			case 't':
				str_ += "\t";
				break;
			case 'n':
				str_ += "\n";
				break;
			case 'r':
				str_ += "\r";
				break;
			case 'u':
				break;
			case '\"':
				str_ += "\"";
				break;
			case '\\':
				str_ += "\\";
				break;
			default:
				return false;
				break;
			}
			return true;
		}

		bool m_bDelete;// �Ƿ���Ҫɾ��
		int m_iArrLen;// ���鳤��
		NodeType m_nodeType;// �ڵ�����
		JsonNode* root_node;// ���ڵ�
	};
}