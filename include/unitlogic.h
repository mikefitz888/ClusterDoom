namespace unitlogic {
	class UnitLogic {
	public:
		std::vector<Unit*> getUnit();
		Unit& createUnit();
		Unit& removeUnit(Unit& unit);
		void step();
		void draw();
	private:
	}
}