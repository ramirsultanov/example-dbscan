#include <array>
#include <vector>
#include <cmath>
#include <iostream>

#include <SFML/Graphics.hpp>

template <typename Pt>
double distance(const Pt& lhs, const Pt& rhs) {
  const auto xdiff = lhs[0] - rhs[0];
  const auto ydiff = lhs[1] - rhs[1];
  return std::sqrt(xdiff * xdiff + ydiff * ydiff);
}

template <typename Points, typename Point, typename Eps>
std::vector<int> neighbours(
    const Points& points,
    const Point& point,
    const Eps& eps
) {
  std::vector<int> ret;
  for (auto i = 0; i < points.size(); i++) {
    if (distance(points[i], point) < eps) {
      ret.push_back(i);
    }
  }
  return ret;
}

template <
    typename Point,
    typename Neighbours,
    typename Mark,
    typename Eps,
    typename MinPts,
    typename Cluster,
    typename Points
> void expand(
    Point&& i,
    Neighbours& neighbour,
    Mark& mark,
    const Eps& eps,
    const MinPts& minPts,
    Cluster& cluster,
    const Points& points
) {
  mark[i] = cluster;
  for (auto j = 0; j < neighbour.size(); j++) {
    if (mark[neighbour[j]] == 0) {
      mark[neighbour[j]] = -2;
      const auto iNeighbour = neighbours(points, points[neighbour[j]], eps);
      if (iNeighbour.size() >= minPts) {
        neighbour.insert(
            std::end(neighbour),
            std::begin(iNeighbour),
            std::end(iNeighbour)
        );
      }
    }
    if (mark[neighbour[j]] < 0) {
      mark[neighbour[j]] = cluster;
    }
  }
}

template <typename Points, typename Eps, typename MinPts>
std::vector<std::array<int, 3>> dbscan(
    Points&& points,
    Eps&& eps,
    MinPts&& minPts
) {
  std::vector<std::array<int, 3>> ret;
  int cluster = 0;
  std::vector<int> mark(points.size(), 0);
  for (auto i = 0u; i < points.size(); i++) {
    if (mark[i] != 0) {
      continue;
    }
    mark[i] = -2;
    auto neighbour = neighbours(points, points[i], eps);
    if (neighbour.size() < minPts) {
      mark[i] = -1;
    } else {
      cluster++;
      expand(i, neighbour, mark, eps, minPts, cluster, points);
    }
  }
  for (auto i = 0; i < points.size(); i++) {
    ret.push_back({points[i][0], points[i][1], mark[i]});
    std::cout <<
        "pt: " << points[i][0] << "," << points[i][1] <<
        "\tmark: " << mark[i] << std::endl;
  }
  return ret;
}

int main()
{
  constexpr auto R = 10.f;
  sf::RenderWindow window(sf::VideoMode(800, 600), "DBSCAN");
  window.setVerticalSyncEnabled(true);
  window.setFramerateLimit(60);
  window.clear(sf::Color(255, 255, 255, 255));
  sf::CircleShape circle(R);
  std::vector<std::array<int, 2>> points;

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      switch (event.type) {
      case sf::Event::Closed:
        window.close();
        break;
      case sf::Event::MouseButtonPressed:
        if (event.mouseButton.button == sf::Mouse::Left) {
          points.push_back({event.mouseButton.x, event.mouseButton.y});
          sf::CircleShape circle(R);
          circle.setPosition(event.mouseButton.x, event.mouseButton.y);
          circle.setFillColor(sf::Color(0, 0, 0, 255));
          circle.setOrigin(circle.getRadius(), circle.getRadius());
          window.draw(circle);
        }
        break;
      case sf::Event::KeyPressed:
        if (event.key.code == sf::Keyboard::Enter) {
          const auto coloured = dbscan(points, 20, 3);
          for (const auto& pt : coloured) {
            sf::CircleShape circle(R);
            circle.setPosition(pt[0], pt[1]);
            circle.setFillColor(sf::Color(((pt[2] << 4) * 255 * 255) | 255));
            circle.setOrigin(circle.getRadius(), circle.getRadius());
            window.draw(circle);
          }
        }
        break;
      default:
        break;
      }
    }
    // window.clear();
    window.display();
  }

  return 0;
}
