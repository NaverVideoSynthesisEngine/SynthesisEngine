import unreal
import os


def load_models():
    # unreal.load_asset('/Game/Models/Female.Female')
    # unreal.load_asset('/Game/Models/Female_Skeleton.Female_Skeleton')
    unreal.load_asset('/Game/Models/Male.Male')
    unreal.load_asset('/Game/Models/Male_Skeleton.Male_Skeleton')


def load_levels():
    unreal.load_asset('/Game/Levels/ToyDatasetLevels/Playground_Toy.Playground_Toy')
    # unreal.load_asset('/Game/Levels/ToyDatasetLevels/Demo_Toy.Demo_Toy')
    unreal.load_asset('/Game/Levels/ToyDatasetLevels/EnvironmentSet_Toy.EnvironmentSet_Toy')


def load_fbx(char, start, end):
    game_fbx_path = '/Game/Animations/Toy/FBX/{}/'.format(char)
    fbx_files_path = "D:/workspace/temp/SynthesisEngine_SC/Content/Animations/Toy/FBX/{}".format(char)
    fbx_files = os.listdir(fbx_files_path)
    for fbx in fbx_files[start:end]:
        unreal.load_asset('{0}{1}.{1}'.format(game_fbx_path, fbx[:-7]))


def load_alembic(char, start, end):
    game_anim_path = '/Game/Animations/Toy/Alembic/{}/'.format(char)
    clothes = ['Crew_neck_t-shirt', 'knitvest', 'sleeveless_t-shirt', 'training_pants']
    anims = os.listdir("D:/workspace/temp/SynthesisEngine_SC/Content/Animations/Toy/Alembic/{}".format(char))
    for anim in anims[start:end]:
        clothes_path = '{}{}/'.format(game_anim_path, anim)
        for cloth in clothes:
            cloth_path = '{}{}/'.format(clothes_path, cloth)
            abc_files_path = "D:/workspace/temp/SynthesisEngine_SC/Content/Animations/Toy/Alembic/{}/{}/{}".format(char, anim, cloth)
            abc_files = os.listdir(abc_files_path)
            for abc_file in abc_files:
                unreal.load_asset("{0}{1}.{1}".format(cloth_path, abc_file[:-7]))


def load_material(name):
    game_mat_path = '/Game/Materials/{}/'.format(name)
    local_mat_path = "D:/workspace/temp/SynthesisEngine_SC/Content/Materials/{}/".format(name)
    assets = os.listdir(local_mat_path)
    for asset in assets:
        unreal.load_asset("{0}{1}.{1}".format(game_mat_path, asset[:-7]))


def init():
    load_models()
    load_levels()
    load_fbx('Male', 0, 1)
    load_alembic('Male', 0, 1)

    load_material('Linen')
    load_material('Canvas')
    # load_material('Denim')
    # load_material('Wool')
