#pragma once
#include <string>
#include <iostream>
#include <queue>
using namespace std;

namespace vcdstarJson
{
	// Json 节点的类型
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
	
	// Json 节点
	typedef struct JsonNode_
	{
		NodeType _type;// 类型
		string _key;// key的string
		double d_val;// 浮点型对象
		long long i_val;// 整形对象
		string str_val;// string对象
		bool b_val;// bool对象
		JsonNode_* obj_val;// json对象节点
		int arrLen;// 数组的长度
		JsonNode_* arr_val;// json数组对象的节点

		JsonNode_* next;// 下一个节点
		JsonNode_* prev;// 上一个节点
		JsonNode_* father;// 父亲节点
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

	// 析构json node 的树对象
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

	// 把node初始化了，恢复值为默认
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

	// 拷贝node的值，引用/指针不拷贝
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

	// 深拷贝node的值
	int deep_copy_node(JsonNode*& src, JsonNode*& dst, JsonNode* father, JsonNode* prev) {
		if (src == nullptr || dst != nullptr)// 必须为空，重头创建，方便递归
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

	// 主要的json对象
	class Json
	{
	public:
		// 构造方法，默认
		Json() : root_node(nullptr), m_bDelete(false), m_iArrLen(-1), m_nodeType(NodeType::type_undefined) {

		}
		
		// 构造方法，从string流中生成对象
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

		// 拷贝构造函数，防止传进来的对象被delete
		Json(const Json& j) {
			m_bDelete = false;// 是否需要删除
			m_iArrLen = j.m_iArrLen;// 数组长度
			m_nodeType = j.m_nodeType;// 节点类型
			root_node = j.root_node;// 根节点
		}

		// 析构方法，如果需要删除根节点
		~Json() {
			if (m_bDelete) {
				release_node(root_node);
				cout << "json release \n";
			}
		}
	
		// 重载[string]，寻找对象
		Json operator[](string _key) {
			if (root_node != nullptr && m_nodeType != NodeType::type_object)
				return Json(nullptr, false, -1, NodeType::type_undefined);
			JsonNode* next_ = root_node;// 用来遍历的节点
			JsonNode* first_undefined = nullptr;// 记录第一个未使用的节点，用于新建节点
			JsonNode* last = nullptr;// 记录最后一个可用节点，用于新建节点
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

		// 重载[int]，寻找对象
		Json operator[](int _index) {
			if (root_node == nullptr)// 数组不能通过[]的方式来创建对象，如果对象没有创建直接返回空
				return Json(nullptr, false, -1, NodeType::type_undefined);
			if (m_nodeType != NodeType::type_array || m_iArrLen <= -1)// 必须是数组类型
				return Json(nullptr, false, -1, NodeType::type_undefined);
			if (_index < 0 || _index >= m_iArrLen)// 防止数组越界
				return Json(nullptr, false, -1, NodeType::type_undefined);

			JsonNode* next_ = root_node;// 用来遍历的节点
			int iIndex = -1;// 计数
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

		// object重载=int
		void operator=(const int _val) {
			long long ll_val = _val;
			operator=(ll_val);
		}

		// object重载=short
		void operator=(const short _val) {
			long long ll_val = _val;
			operator=(ll_val);
		}

		// object重载=long
		void operator=(const long _val) {
			long long ll_val = _val;
			operator=(ll_val);
		}

		// object重载=longlong
		void operator=(const long long _val) {
			if (root_node != nullptr) {
				if (m_nodeType == NodeType::type_object || m_nodeType == NodeType::type_array) {
					if (root_node->father == nullptr)// 当前是根节点对象，不允许修改
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

		// object重载=float
		void operator=(const float _val) {
			double d_val = _val;
			operator=(d_val);
		}

		// object重载=double
		void operator=(const double _val) {
			if (root_node != nullptr) {
				if (m_nodeType == NodeType::type_object || m_nodeType == NodeType::type_array) {
					if (root_node->father == nullptr)// 当前是根节点对象，不允许修改
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

		// object重载=string
		void operator=(const string _val) {
			if (root_node != nullptr) {
				if (m_nodeType == NodeType::type_object || m_nodeType == NodeType::type_array) {
					if (root_node->father == nullptr)// 当前是根节点对象，不允许修改
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

		// object重载=bool
		void operator=(const bool _val) {
			if (root_node != nullptr) {
				if (m_nodeType == NodeType::type_object || m_nodeType == NodeType::type_array) {
					if (root_node->father == nullptr)// 当前是根节点对象，不允许修改
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

		// object重载=null
		void operator=(const void* _val) {
			if (root_node != nullptr) {
				if (m_nodeType == NodeType::type_object || m_nodeType == NodeType::type_array) {
					if (root_node->father == nullptr)// 当前是根节点对象，不允许修改
						return;
					root_node = root_node->father;
				}
				string _key = root_node->_key;
				init_node_val(root_node);
				root_node->_type = NodeType::type_null;
				root_node->_key = _key;
			}
		}

		// object重载=json
		void operator=(Json _val) {
			_val.m_bDelete = false;// 防止函数退出后析构，把传进来的节点值给删除了
			if (root_node == nullptr && _val.root_node != nullptr && (_val.m_nodeType == NodeType::type_array || _val.m_nodeType == NodeType::type_object))// 考虑到直接赋值的情况
			{
				deep_copy_node(_val.root_node, root_node, root_node, nullptr);
				m_bDelete = true;
				m_iArrLen = _val.m_iArrLen;
				m_nodeType = _val.m_nodeType;
				return;
			}
			if (root_node != nullptr && _val.root_node != nullptr && (_val.m_nodeType == NodeType::type_array || _val.m_nodeType == NodeType::type_object)) {
				if (m_nodeType == NodeType::type_object || m_nodeType == NodeType::type_array) {
					if (root_node->father == nullptr)// 当前是根节点对象，不允许修改
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

		// 创建空的object对象
		bool createEmptyObject() {
			if (root_node != nullptr)
				return false;

			root_node = new JsonNode();
			m_nodeType = NodeType::type_object;
			m_bDelete = true;
		}

		// 创建空的array对象
		bool createEmptyArray() {
			if (root_node != nullptr)
				return false;

			root_node = new JsonNode();
			m_nodeType = NodeType::type_array;
			m_bDelete = true;
			m_iArrLen = 0;
		}

		// ------以下是数组的方法--------
		// 数组添加int对象
		bool add(const int _val) {
			long long l_val = _val;
			return add(l_val);
		}

		// 数组添加short对象
		bool add(const short _val) {
			long long l_val = _val;
			return add(l_val);
		}

		// 数组添加long对象
		bool add(const long _val) {
			long long l_val = _val;
			return add(l_val);
		}

		// 数组添加longlong对象
		bool add(const long long _val) {
			JsonNode* node = find_arr_add_node();
			if (node == nullptr)
				return false;
			node->_type = NodeType::type_int;
			node->i_val = _val;
			return true;
		}

		// 数组添加float对象
		bool add(const float _val) {
			double d_val = _val;
			return add(d_val);
		}

		// 数组添加double对象
		bool add(const double _val) {
			JsonNode* node = find_arr_add_node();
			if (node == nullptr)
				return false;
			node->_type = NodeType::type_double;
			node->d_val = _val;
			return true;
		}

		// 数组添加string对象
		bool add(const string _val) {
			JsonNode* node = find_arr_add_node();
			if (node == nullptr)
				return false;
			node->_type = NodeType::type_string;
			node->str_val = _val;
			return true;
		}

		// 数组添加bool对象
		bool add(const bool _val) {
			JsonNode* node = find_arr_add_node();
			if (node == nullptr)
				return false;
			node->_type = NodeType::type_bool;
			node->b_val = _val;
			return true;
		}

		// 数组添加null对象
		bool add() {
			JsonNode* node = find_arr_add_node();
			if (node == nullptr)
				return false;
			node->_type = NodeType::type_null;
			return true;
		}

		// 数组添加json对象
		bool add(Json _val) {
			_val.m_bDelete = false;// 防止函数退出后析构，把传进来的节点值给删除了
			if (_val.root_node == nullptr || (_val.m_nodeType != NodeType::type_array && _val.m_nodeType != NodeType::type_object))// 要增加的节点不能为空,也不能为数组或者对象以外的节点
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

		// 数组insert null方法
		bool insert(int _index) {
			JsonNode* node = find_arr_insert_node(_index);
			if (node == nullptr)
				return false;

			node->_type = NodeType::type_null;
			return true;
		}

		// 数组insert int方法
		bool insert(int _index, const int _val) {
			long long l_val = _val;
			return insert(_index, l_val);
		}

		// 数组insert short方法
		bool insert(int _index, const short _val) {
			long long l_val = _val;
			return insert(_index, l_val);
		}

		// 数组insert long方法
		bool insert(int _index, const long _val) {
			long long l_val = _val;
			return insert(_index, l_val);
		}

		// 数组insert longlong方法
		bool insert(int _index, const long long _val) {
			JsonNode* node = find_arr_insert_node(_index);
			if (node == nullptr)
				return false;

			node->_type = NodeType::type_int;
			node->i_val = _val;
			return true;
		}

		// 数组insert float方法
		bool insert(int _index, const float _val) {
			double d_val = _val;
			return insert(_index, d_val);
		}

		// 数组insert double方法
		bool insert(int _index, const double _val) {
			JsonNode* node = find_arr_insert_node(_index);
			if (node == nullptr)
				return false;

			node->_type = NodeType::type_double;
			node->d_val = _val;
			return true;
		}

		// 数组insert bool方法
		bool insert(int _index, const bool _val) {
			JsonNode* node = find_arr_insert_node(_index);
			if (node == nullptr)
				return false;

			node->_type = NodeType::type_bool;
			node->b_val = _val;
			return true;
		}

		// 数组insert string方法
		bool insert(int _index, const string _val) {
			JsonNode* node = find_arr_insert_node(_index);
			if (node == nullptr)
				return false;

			node->_type = NodeType::type_string;
			node->str_val = _val;
			return true;
		}
		
		// 数组insert json方法
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

		// 转成string对象
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
		// 私有构造方法
		Json(JsonNode* _node, bool bDelete, int iArrLen, NodeType nodetype) {
			root_node = _node;
			m_bDelete = bDelete;
			m_iArrLen = iArrLen;
			m_nodeType = nodetype;
		}

		// 查找数组中可以add操作的节点
		JsonNode* find_arr_add_node() {
			if (root_node == nullptr || m_nodeType != NodeType::type_array)
				return nullptr;
			JsonNode* next = root_node;// 用来找可以add的对象
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

		// 查找数组中可以insert操作的结果
		JsonNode* find_arr_insert_node(int _index) {
			if (root_node == nullptr || m_iArrLen < 0 || m_iArrLen <= _index || m_nodeType != NodeType::type_array)
				return nullptr;

			JsonNode* next = root_node;// 用来找可以add的对象
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
			if (_index == 0)// 防止插入0的时候找不到头节点
				root_node = new_node;

			return new_node;
		}

		// object 转 string 对象
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
						// 处理转义字符
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

		// array 转 string 对象
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
						// 处理转义字符
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

		// 解析json Object对象
		int parse_obj(JsonNode*& node_, string& str_json, int& start_index)
		{
			string tmp;
			JsonNode* index_Node = node_;// 当前的节点
			for (int i = start_index + 1; i < str_json.size(); i++)
			{
				find_jump(str_json, i);
				// 防止是一个空对象
				if (i < str_json.size() && str_json[i] == '}') {
					start_index = i;
					return 0;
				}
				// 解析key
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

				// 处理中间的分号
				if (i < str_json.size() && find_char(str_json, ':', ++i))
					i++;
				else
					return -1;
				// 解析value
				find_jump(str_json, i);
				if (parse_value(index_Node, str_json, i) == -1)
					return -1;
				// 解析后处理
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

		//解析json数组对象
		int parse_arr(JsonNode*& node_, string& str_json, int& start_index, int& iArrLen)
		{
			JsonNode* index_Node = node_;// 当前的节点

			for (int i = start_index + 1; i < str_json.size(); i++)
			{
				find_jump(str_json, i);
				// 防止是一个空数组
				if (i < str_json.size() && str_json[i] == ']') {
					start_index = i;
					return 0;
				}
				find_jump(str_json, i);// 跳过所有空格
				if (parse_value(index_Node, str_json, i) == -1)// 解析值
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

		// 解析value
		int parse_value(JsonNode*& index_Node, string& str_json, int& iIndex)
		{
			string tmp;
			if (iIndex < str_json.size() && str_json[iIndex] == '\"')// string 类型的值
			{
				if (parse_string(str_json, tmp, iIndex)) {
					index_Node->str_val = tmp;
					index_Node->_type = NodeType::type_string;
				}
				else {
					return -1;
				}
			}
			else if (iIndex < str_json.size() && str_json[iIndex] == '{')//对象类型
			{
				index_Node->obj_val = new JsonNode();
				index_Node->_type = NodeType::type_object;
				index_Node->obj_val->father = index_Node;
				if (parse_obj(index_Node->obj_val, str_json, iIndex))
					return -1;
			}
			else if (iIndex < str_json.size() && str_json[iIndex] >= '0' && str_json[iIndex] <= '9') // 数值类型
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
				str_json[iIndex + 3] == 'l') //null类型
			{
				index_Node->_type = NodeType::type_null;
				iIndex += 3;
			}
			else if (iIndex + 4 < str_json.size() && str_json[iIndex] == 'f' &&
				str_json[iIndex + 1] == 'a' &&
				str_json[iIndex + 2] == 'l' &&
				str_json[iIndex + 3] == 's' &&
				str_json[iIndex + 4] == 'e') //false类型
			{
				index_Node->_type = NodeType::type_bool;
				index_Node->b_val = false;
				iIndex += 4;
			}
			else if (iIndex + 3 < str_json.size() && str_json[iIndex] == 't' &&
				str_json[iIndex + 1] == 'r' &&
				str_json[iIndex + 2] == 'u' &&
				str_json[iIndex + 3] == 'e') //true类型
			{
				index_Node->_type = NodeType::type_bool;
				index_Node->b_val = true;
				iIndex += 3;
			}
			else if (iIndex < str_json.size() && str_json[iIndex] == '[')// 数组类型
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

		// 跳过空格和回车，直到查找到指定字符
		bool find_char(string& str_json, char szChar, int& iIndex)
		{
			for (; iIndex < str_json.size(); iIndex++)
			{
				if (str_json[iIndex] == ' ' || str_json[iIndex] == '\n' || str_json[iIndex] == '\r')//跳过空格和回车
					continue;
				else if (str_json[iIndex] == szChar)
					return true;
				else
					return false;
			}
			return false;
		}

		// 跳过空格和回车
		void find_jump(string& str_json, int& iIndex)
		{
			for (; iIndex < str_json.size(); iIndex++)
			{
				if (str_json[iIndex] == ' ' || str_json[iIndex] == '\n' || str_json[iIndex] == '\r')//跳过空格和回车
					continue;
				else
					return;
			}
		}

		//处理string，也可以处理key,参数iIndex必须是第一个"的位置
		bool parse_string(string& str_json, string& str_, int& iIndex)
		{
			for (int i = iIndex; i < str_json.size(); i++)
			{
				if (str_json[i] == '\"') {
					for (int j = i + 1; j < str_json.size(); j++) // 遍历查找key
					{
						if (str_json[j] == '\\' && j + 1 < str_json.size())//处理转义字符
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

		// 处理数值类型，返回数值类型的string
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

		//处理转义字符
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

		bool m_bDelete;// 是否需要删除
		int m_iArrLen;// 数组长度
		NodeType m_nodeType;// 节点类型
		JsonNode* root_node;// 根节点
	};
}