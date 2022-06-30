#ifndef LOCATIONDIRECTIVE_HPP
# define LOCATIONDIRECTIVE_HPP

# include <string>
# include <vector>

class LocationDirective
{
	public:
		LocationDirective();
		~LocationDirective();

		void	ParseLocationDirective();

	private:
		std::pair<int, std::string>	return_;
		std::string					root_;
		bool						autoindex_;
		std::vector<std::string>	indexes_;
};

#endif  // LOCATIONDIRECTIVE_HPP
