#pragma once

#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <map>
#include <boost/locale.hpp>
#include <cctype>


struct Link
{
	std::string protocol;
	std::string host;
	std::string target;

	bool operator==(const Link& l) const
	{
		return protocol == l.protocol
			&& host == l.host
			&& target == l.target;
	}
};

Link SplitLink(std::string url);
std::vector<Link> ExtractLinks(const std::string& html, const Link& entryLink);
std::string CleanHTML(const std::string& html);
std::map<std::string, int> index_words(const std::string& text);
std::string LinkBuilder(const Link& link);