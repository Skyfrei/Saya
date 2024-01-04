import Warcraft
# Can access vector
player_instance = Warcraft.Vec2(2, 2)
print(player_instance.x)

# Can access peasants
slave = Warcraft.Peasant()
print(slave.GetDescription())

# Can access Structures
barrack = Warcraft.Barrack()
print(barrack.GetDescription())

# Can access player
player = Warcraft.Player()
print(player.HasUnit(Warcraft.UnitType.PEASANT))
print(player.gold)
print(player.food)

