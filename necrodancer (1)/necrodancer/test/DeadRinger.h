#include "UnitBase.h"
#include <vector>

enum class DeadRingerState {
	NORMAL,
	READY,
	DASH,
	DASHING,
	PHASE2_IDLE,
	SKILL2_READY,
	PHASE2_VULNERABLE
};

class DeadRinger : public UnitBase
{
public:
	DeadRinger(float hp, float attack, int movedistance, Vector2 pos, ObjectTag tag);
	virtual ~DeadRinger() {}

	virtual void Move() override;
	virtual void Attack(UnitBase& Target) override;
	virtual void TakeDamage(float atk) override;
	virtual void Die() override;
	virtual void Update() override;

	DeadRingerState GetState() const { return m_state; }
	bool IsPhase2() const { return m_isPhase2; }
	const std::vector<Vector2>& GetSkillPath() const { return m_skillPath; }
	void ClearSkillPath() { m_skillPath.clear(); }

private:
	Vector2 m_spawnPos;
	DeadRingerState m_state;
	Vector2 m_dashDir;
	std::vector<Vector2> m_skillPath;


	bool m_isPhase2;
	bool m_isInvincible;
	int  m_explosionCount;
	int  m_phase2Timer;
	int  m_phantomTimer;
};
