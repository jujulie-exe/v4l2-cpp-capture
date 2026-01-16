NAME = camera_test

SRCS = src/Camera.cpp src/main.cpp
OBJS = $(SRCS:.cpp=.o)
DEPS = $(OBJS:.o=.d)
INC = -I include

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -Wshadow -I include -MMD -MP

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(OBJS) $(DEPS)

fclean: clean
	rm -f $(NAME)

re: fclean all

-include $(DEPS)
PHONY: all clean fclean re