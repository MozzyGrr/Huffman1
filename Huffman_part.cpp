#include <iostream>
#include <array>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <queue>
#include <cstring>
#include <string>


#define HUFF_PREFIX "<huff>\n"
#define HUFF_POSTFIX "</huff>\n"
#define HUFF_PREFIX_SIZE strlen(HUFF_PREFIX)
#define HUFF_POSTFIX_SIZE strlen(HUFF_POSTFIX)


struct Node
{
	char c;
	std::size_t frequency;
	Node* left;
	Node* right;
};

void lessPush(std::vector<Node*>& node, Node* newNode)
{
	auto it = node.begin();
	while (it != node.end() && (*it)->frequency > newNode->frequency)
		++it;

	node.insert(it, newNode);
}


Node* getHuffmanTree(const std::array<std::size_t, UCHAR_MAX>& freq)
{
	
	std::vector<Node*> pq;
	for (std::size_t i = 0; i < UCHAR_MAX; ++i)
	{
		const auto f = freq[i];
		if (f != 0)
			lessPush(pq, new Node{ static_cast<char>(i), f, nullptr, nullptr });
	}

	
	if (pq.size() == 1)
	{
		auto node = pq.back();
		return new Node{ '\0', node->frequency, node, nullptr };
	}

	
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

	

	for (const auto& c : data)
		encoded += huffmanCode.at(c);

	return encoded;
}
std::string decode(const std::string& data)
{
	if (data.size() < HUFF_PREFIX_SIZE + HUFF_POSTFIX_SIZE ||
		std::memcmp(data.data(), HUFF_PREFIX, HUFF_PREFIX_SIZE) != 0)
	{
		throw std::runtime_error("Not encoded file");
	}

	std::array<std::size_t, UCHAR_MAX> freq{};
	bool closed = false;
	std::size_t i = HUFF_PREFIX_SIZE;
	while (i + HUFF_POSTFIX_SIZE <= data.size())
	{
		if (std::memcmp(data.data() + i, HUFF_POSTFIX, HUFF_POSTFIX_SIZE) == 0)
		{
			i += HUFF_POSTFIX_SIZE;
			closed = true;
			break;
		}

		const unsigned char uc = data[i];
		i += 2;

		char* e;
		const auto n = std::strtoull(data.data() + i, &e, 10);

		freq[uc] = n;
		i = e - data.data() + 1;
	}

	if (!closed)
		throw std::runtime_error("Expected close tag.");

	auto root = getHuffmanTree(freq);

	std::string decoded;
	while (i < data.size())
	{
		for (auto node = root; i <= data.size(); ++i)
		{
			auto toProcess = data[i] == '0' ? node->left : node->right;
			if (!toProcess)
			{
				decoded += node->c;
				break;
			}
			node = toProcess;
		}
	}
	return decoded;
}
int main()
{
	std::cout << "Select mode: encode | decode | cmp\n";

	std::string command;
	std::cin >> command;

	if (command == "encode" || command == "-e")
	{
		std::string filename;
		std::cout << "Filename:";
		std::cin >> filename;

		std::ifstream ifs(filename);
		if (!ifs.is_open())
		{
			std::cerr << "Can not open \"" << filename << "\".";
			return -1;
		}

		const auto encoded = encode({ std::istreambuf_iterator<char>(ifs),
		std::istreambuf_iterator<char>() });

		std::ofstream ofs("encoded.txt");
		if (!ofs.is_open())
		{
			std::cerr << "Unable to create out file.";
			return -2;
		}

		ofs << encoded;
	}
	else if (command == "decode" || command == "-d")
	{
		std::string filename;
		std::cout << "Filename:";
		std::cin >> filename;

		std::ifstream ifs(filename);
		if (!ifs.is_open())
		{
			std::cerr << "Can not open \"" << filename << "\".";
			return -1;
		}

		const auto decoded = decode({ std::istreambuf_iterator<char>(ifs),
		std::istreambuf_iterator<char>() });	

		std::ofstream ofs("decoded.txt");
		if (!ofs.is_open())
		{
			std::cerr << "Unable to create out file.";
			return -2;
		}

		ofs << decoded;
	}
	else if (command == "cmp" || command == "-c")
	{
		std::string filename1;
		std::cout << "Filename 1:";
		std::cin >> filename1;

		std::string filename2;
		std::cout << "Filename 2:";
		std::cin >> filename2;

		std::ifstream ifs1(filename1);
		if (!ifs1.is_open())
		{
			std::cerr << "Can not open \"" << filename1 << "\".";
			return -1;
		}

		std::ifstream ifs2(filename2);
		if (!ifs2.is_open())
		{
			std::cerr << "Can not open \"" << filename2 << "\".";
			return -1;
		}

		const auto isEq = std::equal(std::istreambuf_iterator<char>(ifs1.rdbuf()),
			std::istreambuf_iterator<char>(),
			std::istreambuf_iterator<char>(ifs2.rdbuf()));

		std::cout << "Files are " << (isEq ? "" : "not ") << "equal.";
	}
	else
	{
		std::cerr << "Invalid command.";
	}

	return 0;
}



