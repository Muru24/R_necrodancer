#pragma once
#include "UnitBase.h"

#include <vector>

class DeadRingerPhantom : public UnitBase
{
public:
	DeadRingerPhantom(Vector2 pos, Vector2 dashDir);
	virtual ~DeadRingerPhantom() {}

	virtual void Move() override;
	virtual void Update() override;
	virtual void TakeDamage(float atk) override;
	virtual void Die() override;

	const std::vector<Vector2>& GetSkillPath() const { return m_skillPath; }

private:
	Vector2 m_dashDir;
	bool m_hasStartedDash;
	std::vector<Vector2> m_skillPath;
};
