#include "link.h"

Link SplitLink(std::string url)
{
	Link link;
	if (url.find("https://") == 0)
	{
		link.protocol = "https";
		url.erase(0, 8);
	}
	else if (url.find("http://") == 0)
	{
		link.protocol = "http";
		url.erase(0, 7);
	}
	else
	{
		link.protocol = "";
	}

	size_t pos = url.find('/');
	if (pos != std::string::npos)
	{
		link.host = url.substr(0, pos);
		link.target = url.substr(pos);
	}
	else
	{
		link.host = url;
		link.target = "/";
	}

	return link;
}

std::vector<Link> ExtractLinks(const std::string& html, const Link& entryLink)
{
	std::vector<Link> links;
	std::vector<std::string> link;
	std::regex link_regex(R"(<a\s+href=["']([^"']+)["'])");
	std::smatch match;
	
	std::string::const_iterator search_start(html.cbegin());
	while (std::regex_search(search_start, html.cend(), match, link_regex))
	{
		link.push_back(match[1].str());
		search_start = match.suffix().first;
	}

	for (const auto& el : link)
	{
		Link tmp = SplitLink(el);
		if (tmp.protocol == "")
		{
			tmp.protocol = entryLink.protocol;
			tmp.host = entryLink.host;
		}
		links.push_back(tmp);
	}
	return links;
}


std::string CleanHTML(const std::string& html)
{
	std::string cleaned = html;
	
	cleaned = std::regex_replace(cleaned, std::regex("<script[^>]*?>[\\s\\S]*?</script>", std::regex::icase), " ");
	cleaned = std::regex_replace(cleaned, std::regex("<style[^>]*?>[\\s\\S]*?</style>", std::regex::icase), " ");

	cleaned = std::regex_replace(cleaned, std::regex("<[^>]+>"), " ");

	std::replace_if(cleaned.begin(), cleaned.end(),
		[](char c) { return c == '\'' || c == '-'; }, ' ');

	cleaned = std::regex_replace(cleaned, std::regex("\\s+"), " ");
	cleaned = std::regex_replace(cleaned, std::regex("^\\s+|\\s+$"), "");
	
	cleaned.erase(std::remove_if(cleaned.begin(), cleaned.end(),
								[](unsigned char c) {return std::ispunct(c); }),
				cleaned.end());

	return cleaned;
}

std::map<std::string, int> index_words(const std::string& text)
{
	std::map<std::string, int> word_count;
	std::istringstream iss(text);
	std::string word;

	while (iss >> word)
	{
		if (word.size() < 3 || word.size() > 32) continue;
		std::transform(word.begin(), word.end(), word.begin(),
			[](unsigned char c) { return std::tolower(c); });
		word_count[word]++;
	}
	return word_count;
}

std::string LinkBuilder(const Link& link)
{
	return link.protocol + "://" + link.host + link.target;
}
