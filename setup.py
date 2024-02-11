import Warcraft
# Can access vector
player_instance = Warcraft.Vec2(3, 2)
print(player_instance.x)

# Can access peasants
slave = Warcraft.Footman()
print(slave.GetDescription())

# Can access Structures
barrack = Warcraft.TownHall()
print(barrack.GetDescription())

# Can access player
player = Warcraft.Player()
#print(player.units.size)
print(player.food)

enemy = Warcraft.Player()

# Can access manager
manager = Warcraft.Manager()
print(manager.player.food)

