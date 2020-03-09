import unreal
import os
from random import randint

is_mac = True
is_blade = False
is_naver = False

if is_mac:
    content_dir = "/Users/chan/SynthesisEngine/Content"

elif is_blade:
    content_dir = "C:/User/ehwjs/workspace/SynthesisEngine/Content"

elif is_naver:
    content_dir = "D:/workspace/SynthesisEngine/Content"

def load_models():
    unreal.load_asset('/Game/Models/Female.Female')
    unreal.load_asset('/Game/Models/Female_Skeleton.Female_Skeleton')
    unreal.load_asset('/Game/Models/Male.Male')
    unreal.load_asset('/Game/Models/Male_Skeleton.Male_Skeleton')
    unreal.load_asset('/Game/Models/Adam.Adam')
    unreal.load_asset('/Game/Models/Adam_Skeleton.Adam_Skeleton')
    unreal.load_asset('/Game/Models/Elizabeth.Elizabeth')
    unreal.load_asset('/Game/Models/Elizabeth_Skeleton.Elizabeth_Skeleton')
    unreal.load_asset('/Game/Models/Shannon.Shannon')
    unreal.load_asset('/Game/Models/Shannon_Skeleton.Shannon_Skeleton')
    unreal.load_asset('/Game/Models/Sophie.Sophie')
    unreal.load_asset('/Game/Models/Sophie_Skeleton.Sophie_Skeleton')


def load_levels():
    unreal.load_asset('/Game/Levels/ToyDatasetLevels/Playground_Toy.Playground_Toy')
    unreal.load_asset('/Game/Levels/ToyDatasetLevels/Modern_Apartment_Toy.Modern_Apartment_Toy')
    unreal.load_asset('/Game/Levels/ToyDatasetLevels/EnvironmentSet_Toy.EnvironmentSet_Toy')
    unreal.load_asset('/Game/Levels/ToyDatasetLevels/Demo_Toy.Demo_Toy')


def load_fbx(char, start, end):
    game_fbx_path = '/Game/Animations/Toy/FBX/{}'.format(char)
    fbx_files_path = "{}/Animations/Toy/FBX/{}".format(content_dir, char)
    fbx_files = os.listdir(fbx_files_path)
    for fbx in fbx_files[start:end]:
        unreal.load_asset('{0}/{1}.{1}'.format(game_fbx_path, fbx[:-7]))


def load_fbx2(char, indices):
    game_fbx_path = '/Game/Animations/Toy/FBX/{}'.format(char)
    fbx_files_path = "{}/Animations/Toy/FBX/{}".format(content_dir, char)
    fbx_files = os.listdir(fbx_files_path)
    fbx_files2 = [fbx_files[i] for i in indices]
    for fbx in fbx_files2:
        unreal.load_asset('{0}/{1}.{1}'.format(game_fbx_path, fbx[:-7]))


def load_alembic(char, start, end):
    game_anim_path = '/Game/Animations/Toy/Alembic/{}'.format(char)
    clothes = ['Crew_neck_t-shirt', 'knitvest', 'sleeveless_t-shirt', 'training_pants']
    anims = os.listdir("{}/Animations/Toy/Alembic/{}".format(content_dir, char))
    for anim in anims[start:end]:
        clothes_path = '{}/{}'.format(game_anim_path, anim)
        for cloth in clothes:
            cloth_path = '{}/{}'.format(clothes_path, cloth)
            abc_files_path = "{}/Animations/Toy/Alembic/{}/{}/{}".format(content_dir, char, anim, cloth)
            abc_files = os.listdir(abc_files_path)
            for abc_file in abc_files:
                unreal.load_asset("{0}/{1}.{1}".format(cloth_path, abc_file[:-7]))


def load_alembic2(char, indices):
    game_anim_path = '/Game/Animations/Toy/Alembic/{}'.format(char)
    clothes = ['Crew_neck_t-shirt', 'knitvest', 'sleeveless_t-shirt', 'training_pants']
    anims = os.listdir("{}/Animations/Toy/Alembic/{}".format(char))
    anims2 = [anims[i] for i in indices]
    for anim in anims2:
        clothes_path = '{}/{}'.format(game_anim_path, anim)
        for cloth in clothes:
            cloth_path = '{}/{}'.format(clothes_path, cloth)
            abc_files_path = "{}/Animations/Toy/Alembic/{}/{}/{}".format(content_dir, char, anim, cloth)
            abc_files = os.listdir(abc_files_path)
            for abc_file in abc_files:
                unreal.load_asset("{0}/{1}.{1}".format(cloth_path, abc_file[:-7]))


def load_material(name, material=randint(1, 30)):
    game_mat_path = '/Game/Materials/{}'.format(name)
    local_mat_path = "{}/Materials/{}".format(name)
    assets = os.listdir(local_mat_path)
    asset = assets[material]
    unreal.load_asset("{0}/{1}.{1}".format(game_mat_path, asset[:-7]))


def load_materials(name, materials):
    game_mat_path = '/Game/Materials/{}'.format(name)
    local_mat_path = "{}/Materials/{}".format(name)
    assets = os.listdir(local_mat_path)
    assets2 = [assets[i] for i in materials]
    for asset in assets2:
        unreal.load_asset("{0}/{1}.{1}".format(game_mat_path, asset[:-7]))


def init():
    load_models()
    load_levels()
    indices = [31, 43, 59] + list(range(68, 80)) + [84, 85]
    # indices = [31, 43, 59]

    load_fbx2('Male', indices)
    load_alembic2('Male', indices)
    load_fbx2('Female', indices)
    load_alembic2('Female', indices)
    load_fbx2('Adam', indices)
    load_fbx2('Elizabeth', indices)
    load_fbx2('Shannon', indices)
    load_fbx2('Sophie', indices)

    load_materials('Linen', list(range(14)))
    load_materials('Denim', list(range(14)))
    # load_material('Canvas')
    # load_material('Wool')
