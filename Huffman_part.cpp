#include <iostream>
#include <array>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <queue>
#include <cstring>
#include <string>

setlocale(LC_ALL, "Rus");

/// ������� � �������� ����� � ��������� � �������������� �����
#define HUFF_PREFIX "<huff>\n"
#define HUFF_POSTFIX "</huff>\n"
#define HUFF_PREFIX_SIZE strlen(HUFF_PREFIX)
#define HUFF_POSTFIX_SIZE strlen(HUFF_POSTFIX)

/// ���� ��� ������ ��������
struct Node
{
	char c;
	std::size_t frequency;
	Node* left;
	Node* right;
};

/**
 * @brief ������� ������ ��������
 * @param root ������ ������
 */
void deleteTree(Node* root)
{
	/**
	 * @brief ��������� � ������� ����� � ������ ������� �������� � ������� � ������� ������� ���� ������� �� �����
	 */
	std::queue<Node*> rq;
	rq.push(root);

	while (!rq.empty())
	{
		auto tmp = rq.front();
		rq.pop();

		if (tmp)
		{
			if (tmp->left)
				rq.push(tmp->left);

			if (tmp->right)
				rq.push(tmp->right);
		}

		delete tmp;
	}
}

/**
 * @brief ������� � ����������� ��������
 * @param node ������ ��� ��������
 * @param newNode ���� ��� �������
 */
void lessPush(std::vector<Node*>& node, Node* newNode)
{
	auto it = node.begin();
	while (it != node.end() && (*it)->frequency > newNode->frequency)
		++it;

	node.insert(it, newNode);
}

/**
 * @brief ������� ������ ��������
 * @param freq ������� ���� ������� ������
 * @return ������ ������ ��������
 */
Node* getHuffmanTree(const std::array<std::size_t, UCHAR_MAX>& freq)
{
	/**
	 * @brief ������� ������ ��� �������� �� ������� ������
	 */
	std::vector<Node*> pq;
	for (std::size_t i = 0; i < UCHAR_MAX; ++i)
	{
		const auto f = freq[i];
		if (f != 0)
			lessPush(pq, new Node{ static_cast<char>(i), f, nullptr, nullptr });
	}

	/**
	 * @brief ��� ���������� ������ ���� � ������� ������ ���� ������� - ����� �� ����� ������ � ������ �����
	 */
	if (pq.size() == 1)
	{
		auto node = pq.back();
		return new Node{ '\0', node->frequency, node, nullptr };
	}

	/**
	 * @brief ���� ������� �� ����� ����� 2 ������� ��������(� ������� ��������)
	 * � ��������� ����� ������� ��� ������� - ����� 2 ������, ����� - ������ ������, ������ - ������ ������
	 */
	while (pq.size() > 1)
	{
		auto left = pq.back();
		pq.pop_back();

		auto right = pq.back();
		pq.pop_back();

		lessPush(pq, new Node{ '\0', left->frequency + right->frequency, left, right });
	}

	return !pq.empty() ? pq.back() : nullptr;
}


std::unordered_map<char, std::string> getHuffmanCodes(Node* root)
{
	std::unordered_map<char, std::string> huffmanCodes;

	std::queue<std::pair<Node*, std::string>> hcq;
	hcq.push({ root, "" });

	while (!hcq.empty())
	{
		auto tmp = hcq.front();
		hcq.pop();

		auto node = tmp.first;
		if (node)
		{
			if (!node->left && !node->right)
			{
				huffmanCodes[node->c] = tmp.second;
			}
			else
			{
				hcq.push({ tmp.first->left, tmp.second + '0' });
				hcq.push({ tmp.first->right, tmp.second + '1' });
			}
		}
	}

	return huffmanCodes;
}


std::string encode(const std::string& data)
{
	std::array<std::size_t, UCHAR_MAX> freq{};
	for (const auto& c : data)
		++freq[static_cast<unsigned char>(c)];

	std::string encoded;
	encoded += HUFF_PREFIX;

	for (std::size_t i = 0; i < UCHAR_MAX; ++i)
	{
		const auto f = freq[i];
		if (f != 0)
			encoded += static_cast<char>(i) + std::string(" ") + std::to_string(f) + '\n';
	}

	encoded += HUFF_POSTFIX;

	auto root = getHuffmanTree(freq);

	const auto huffmanCode = getHuffmanCodes(root);

	deleteTree(root);

	for (const auto& c : data)
		encoded += huffmanCode.at(c);

	return encoded;
}




