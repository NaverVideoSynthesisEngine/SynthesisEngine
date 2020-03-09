import unreal
import os

is_mac = False

def load_models():
    unreal.load_asset('/Game/Models/Female.Female')
    unreal.load_asset('/Game/Models/Female_Skeleton.Female_Skeleton')

def load_models(name):
    model_path = "/Game/Models/{0}.{1}".format(name, name)
    skel_path = "/Game/Models/{0}_Skeleton.{1}_Skeleton".format(name, name)
    unreal.load_asset(model_path)
    unreal.load_asset(skel_path)

def load_levels():
    unreal.load_asset('/Game/Levels/ToyDatasetLevels/Playground_Toy.Playground_Toy')
    unreal.load_asset('/Game/Levels/ToyDatasetLevels/Demo_Toy.Demo_Toy')
    unreal.load_asset('/Game/Levels/ToyDatasetLevels/EnvironmentSet_Toy.EnvironmentSet_Toy')

def load_fbx(char, start, end):
    game_fbx_path = '/Game/Animations/Toy/FBX/{}/'.format(char)
    if True:
        fbx_files_path = "/Users/chan/SynthesisEngine/Content/Animations/Toy/FBX/{}".format(char)
    else:
        fbx_files_path = "D:/ChangHun/sourcetree/SynthesisEngine/Content/Animations/Toy/FBX/{}".format(char)
    fbx_files = os.listdir(fbx_files_path)
    for fbx in fbx_files[start:end]:
        unreal.load_asset('{0}{1}.{1}'.format(game_fbx_path, fbx[:-7]))


def load_alembic(char, start, end):
    game_anim_path = '/Game/Animations/Toy/Alembic/{}/'.format(char)
    clothes = ['Crew_neck_t-shirt', 'knitvest', 'sleeveless_t-shirt', 'training_pants']
    if is_mac:
        anims = os.listdir("/Users/chan/SynthesisEngine/Content/Animations/Toy/Alembic/{}".format(char))
    else:
        anims = os.listdir("D:/ChangHun/sourcetree/SynthesisEngine/Content/Animations/Toy/Alembic/{}".format(char))
    for anim in anims[start:end]:
        clothes_path = '{}{}/'.format(game_anim_path, anim)
        for cloth in clothes:
            cloth_path = '{}{}/'.format(clothes_path, cloth)
            if is_mac:
                abc_files_path = "/Users/chan/SynthesisEngine/Content/Animations/Toy/Alembic/{}/{}/{}".format(char, anim, cloth)
            else :
                abc_files_path = "D:/ChangHun/sourcetree/SynthesisEngine/Content/Animations/Toy/Alembic/{}/{}/{}".format(char, anim, cloth)
            abc_files = os.listdir(abc_files_path)
            for abc_file in abc_files:
                unreal.load_asset("{0}{1}.{1}".format(cloth_path, abc_file[:-7]))


def load_material(name):
    game_mat_path = '/Game/Materials/{}/'.format(name)
    if is_mac:
        local_mat_path = "/Users/chan/SynthesisEngine/Content/Materials/{}/".format(name)
    else:
        local_mat_path = "D:/ChangHun/sourcetree/SynthesisEngine/Content/Materials/{}/".format(name)

    assets = os.listdir(local_mat_path)
    for asset in assets:
        unreal.load_asset("{0}{1}.{1}".format(game_mat_path, asset[:-7]))


def init():
    load_models()
    load_levels()
    load_fbx('Male', 5, 6)
    load_alembic('Male', 5, 6)

    load_material('Linen')
    load_material('Canvas')
    load_material('Denim')
    load_material('Wool')

def debug_init():
    global is_mac
    is_mac = True
    load_models('Male')
    load_models('Female')
    
    load_fbx('Male', 0, 2)
    load_fbx('Female', 0, 2)
    unreal.load_asset('/Game/Levels/ToyDatasetLevels/Playground_Toy.Playground_Toy')
    unreal.load_asset('/Game/Levels/ToyDatasetLevels/EnvironmentSet_Toy.EnvironmentSet_Toy')
    load_material('Unrealistic')
    is_mac = False


