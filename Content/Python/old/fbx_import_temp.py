# It's not using, because It run sync.
import unreal
import os
import time
import argparse


def checkExist(char_name, asset_name):
    chars_folder_dir = "D:/ChangHun/sourcetree/SynthesisEngine/Content/Animations/NewFBX"
    uasset_dir = os.path.join(chars_folder_dir, char_name)
    uasset_path = os.path.join(uasset_dir, "{}.uasset".format(asset_name))
    return os.path.isfile(uasset_path)


def renameAsset(from_path='', to_path=''):
    return unreal.EditorAssetLibrary.rename_asset(
        source_asset_path=from_path, destination_asset_path=to_path)


def deleteAsset(path=''):
    return unreal.EditorAssetLibrary.delete_asset(asset_path_to_delete=path)


def saveAsset(path='', force_save=True):
    return unreal.EditorAssetLibrary.save_asset(asset_to_save=path, only_if_is_dirty=not force_save)


def buildImportTask(filename='', destination_path='', options=None):
    task = unreal.AssetImportTask()
    task.set_editor_property('automated', True)
    task.set_editor_property('destination_name', '')
    task.set_editor_property('destination_path', destination_path)
    task.set_editor_property('filename', filename)
    task.set_editor_property('replace_existing', False)
    task.set_editor_property('save', True)
    task.set_editor_property('options', options)
    return task


def executeImportTasks(tasks=[]):
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks(tasks)
    # imported_asset_paths = []
    # for task in tasks:
    #     for path in task.get_editor_property('imported_object_paths'):
    #         imported_asset_paths.append(path)
    # return imported_asset_paths


def buildAnimationImportOptions(skeleton_path='', physics_path=''):
    options = unreal.FbxImportUI()
    # unreal.FbxImportUI
    options.set_editor_property('import_animations', True)
    options.skeleton = unreal.load_asset(skeleton_path)
    options.physics_asset = unreal.load_asset(physics_path)
    # unreal.FbxMeshImportData
    options.anim_sequence_import_data.set_editor_property('import_translation', unreal.Vector(0.0, 0.0, 0.0))
    options.anim_sequence_import_data.set_editor_property('import_rotation', unreal.Rotator(0.0, 0.0, 0.0))
    options.anim_sequence_import_data.set_editor_property('import_uniform_scale', 1.0)
    # unreal.FbxAnimSequenceImportData
    options.anim_sequence_import_data.set_editor_property('animation_length', unreal.FBXAnimationLengthImportType.FBXALIT_EXPORTED_TIME)
    options.anim_sequence_import_data.set_editor_property('remove_redundant_keys', True)
    return options


def Run(char, fbx_folder_path, game_folder_path, fbx_file):
    # check already importd
    asset_name = fbx_file[:-4]
    if checkExist(char, asset_name):
        unreal.log("pass {}".format(asset_name))
        return

    fbx_file_path = os.path.join(fbx_folder_path, fbx_file)
    fbx_options = buildAnimationImportOptions('/Game/Models/NewModels/{0}_Skeleton.{0}_Skeleton'.format(char), '/Game/Models/NewModels/{0}_material/{0}_PhysicsAsset.{0}_PhysicsAsset'.format(char))
    import_task = buildImportTask(fbx_file_path, game_folder_path, fbx_options)
    executeImportTasks([import_task])

    # postprocess
    res_skel_path = game_folder_path + "/{0}.{0}".format(asset_name)
    anim_path = game_folder_path + "/{0}_Anim.{0}_Anim".format(asset_name)
    deleteAsset(res_skel_path)
    renameAsset(anim_path, res_skel_path)
    saveAsset(res_skel_path)
    time.sleep(1)


if __name__ == '__main__':
    unreal.log("pass")
    parser = argparse.ArgumentParser()
    parser.add_argument("char")
    parser.add_argument("fbx_folder_path")
    parser.add_argument("game_folder_path")
    parser.add_argument("fbx_file")
    args = parser.parse_args()
    # Run(args.char, args.fbx_folder_path, args.game_folder_path, args.fbx_file)
