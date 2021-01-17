#include <iostream>
#include <View.h>

int main()
{
	auto view = getView();
	std::vector<Visual> v;
	v.emplace_back(Color::RED, 0.f, 0.f);
	while (view->isActive()) {
		view->draw(v);
	}
	return 0;
}
