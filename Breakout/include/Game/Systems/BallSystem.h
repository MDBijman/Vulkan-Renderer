#pragma once
#include "../ECSConfig.h"

namespace systems
{
	class ball_system : public MySystem
	{
		events::subscriber<events::collision>& subscriber;
	public:
		ball_system(events::subscriber<events::collision>& sub) : subscriber(sub) {}

		void update(ecs_manager& ecs) override
		{
			auto balls = ecs.filterEntities<ecs::filter<components::ball>>();

			while (subscriber.size() > 0)
			{
				std::shared_ptr<const events::collision> collision = subscriber.pop();

				if (balls.entities.find(collision->a) != balls.entities.end()) bounce(ecs, collision->b, collision->a);
				if (balls.entities.find(collision->b) != balls.entities.end()) bounce(ecs, collision->a, collision->b);
			}
		}

		void bounce(ecs_manager& ecs, uint32_t block, uint32_t ball)
		{
			auto paddles = ecs.filterEntities<ecs::filter<components::paddle>>();

			std::array<glm::vec2, 4> other_cube = {
					glm::vec2{ -.5f, -.5f },
					{ -.5f, .5f },
					{ .5f, -.5f },
					{ .5f, .5f },
			};

			auto ball_cube = other_cube;

			auto scale = [](std::array<glm::vec2, 4>& cube, const glm::vec2& offset) {
				for (auto& component : cube)
					component *= offset;
			};
			auto move = [](std::array<glm::vec2, 4>& cube, const glm::vec2& offset) {
				for (auto& component : cube)
					component += offset;
			};

			auto& other_pos = ecs.getComponent<Components::Position2D>(block);
			auto& other_scale = ecs.getComponent<Components::Scale2D>(block);
			auto& ball_position = ecs.getComponent<Components::Position2D>(ball);
			auto& ball_scale = ecs.getComponent<Components::Scale2D>(ball);
			auto& ball_velocity = ecs.getComponent<Components::Velocity2D>(ball);

			scale(other_cube, other_scale.scale);
			move(other_cube, other_pos.pos);
			scale(ball_cube, ball_scale.scale);
			move(ball_cube, ball_position.pos);

			auto intersects = [](const std::array<glm::vec2, 4>& cube, const glm::vec2& pos) {
				auto overlapX = pos.x - cube[0].x;
				auto overlapX2 = cube[2].x - pos.x;
				auto overlapY = pos.y - cube[0].y;
				auto overlapY2 = cube[3].y - pos.y;

				if (overlapX > 0 && overlapY > 0 && overlapX2 > 0 && overlapY2 > 0)
					return glm::vec2(glm::min(overlapX, overlapX2), glm::min(overlapY, overlapY2));

				return glm::vec2();
			};

			for (auto ball_vertex : ball_cube)
			{
				if (auto overlap = intersects(other_cube, ball_vertex); overlap != glm::vec2(0, 0))
				{
					// Left/Right
					if (overlap.x < overlap.y)
					{
						// Left
						if (ball_vertex.x < other_pos.pos.x)
						{
							auto normal = glm::vec2(-overlap.x, 0);
							if (glm::dot(normal, ball_velocity.vel) < 0)
								ball_velocity.vel.x *= -1;
						}
						// Right
						else
						{
							auto normal = glm::vec2(overlap.x, 0);
							if (glm::dot(normal, ball_velocity.vel) < 0)
								ball_velocity.vel.x *= -1;
						}
					}
					// Top/Bottom
					else
					{
						// Top
						if (ball_vertex.y < other_pos.pos.y)
						{
							auto normal = glm::vec2(0, -overlap.y);
							if (glm::dot(normal, ball_velocity.vel) < 0)
							{
								auto l = glm::length(ball_velocity.vel);
								if (paddles.entities.find(block) != paddles.entities.end())
								{
									auto paddle_offset = ball_position.pos.x - other_pos.pos.x;
									ball_velocity.vel.x += paddle_offset;
								}
								ball_velocity.vel.y *= -1;
								auto normalized = glm::normalize(ball_velocity.vel);
								ball_velocity.vel = normalized;
								ball_velocity.vel *= l;
							}
						}
						// Bottom
						else
						{
							auto normal = glm::vec2(0, overlap.y);
							if (glm::dot(normal, ball_velocity.vel) < 0)
								ball_velocity.vel.y *= -1;
						}
					}
				}
			}
		}
	};
}