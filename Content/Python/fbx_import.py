import unreal
import os
import time


def checkExist(char_name, asset_name):
    #chars_folder_dir = "D:/ChangHun/sourcetree/SynthesisEngine/Content/Animations/NewFBX"
    chars_folder_dir = "D:/ChangHun/sourcetree/SynthesisEngine/Content/Animations/Toy/FBX"
    
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
    options.anim_sequence_import_data.set_editor_property('use_default_sample_rate', False)
    options.anim_sequence_import_data.set_editor_property('custom_sample_rate', 30)
    options.anim_sequence_import_data.set_editor_property('animation_length', unreal.FBXAnimationLengthImportType.FBXALIT_EXPORTED_TIME)
    options.anim_sequence_import_data.set_editor_property('remove_redundant_keys', True)
    return options


def Run():
    print("import start.")
    game_path = r"/Game"
    #fbx_folder_path = "D:\\ChangHun\\sourcetree\\SynthesisEngine\\MixamoDownload\\DownloadedAnimations"
    #fbx_folder_path = "D:\\ChangHun\\sourcetree\\SynthesisEngine\\ObjSeqUploadAndConvert\\2.Workspace\\FBX\\temp"
    fbx_folder_path = "D:\\ChangHun\\sourcetree\\SynthesisEngine\\ObjSeqUploadAndConvert\\2.Workspace\\FBX"
    chars = os.listdir(fbx_folder_path)
    print(chars)
    for char in chars:
        cur_fbx_folder_path = os.path.join(fbx_folder_path, char)
        #cur_char_game_folder_path = game_path + "/Animations/NewFBX/" + char
        cur_char_game_folder_path = game_path + "/Animations/Toy/FBX/" + char
        fbx_files = os.listdir(cur_fbx_folder_path)
        for i, fbx_file in enumerate(fbx_files):
            # check already importd
            asset_name = fbx_file[:-4]
            if checkExist(char, asset_name):
                unreal.log("pass {}, {}".format(char, asset_name))
                continue

            fbx_file_path = os.path.join(cur_fbx_folder_path, fbx_file)
            unreal.log("import {}".format(fbx_file_path))
            fbx_options = buildAnimationImportOptions('/Game/Models/NewModels/{0}_Skeleton.{0}_Skeleton'.format(char), '/Game/Models/NewModels/{0}_material/{0}_PhysicsAsset.{0}_PhysicsAsset'.format(char))
            import_task = buildImportTask(fbx_file_path, cur_char_game_folder_path, fbx_options)
            executeImportTasks([import_task])

            # postprocess
            res_skel_path = cur_char_game_folder_path + "/{0}.{0}".format(asset_name)
            anim_path = cur_char_game_folder_path + "/{0}_Anim.{0}_Anim".format(asset_name)
            deleteAsset(res_skel_path)
            renameAsset(anim_path, res_skel_path)
            saveAsset(res_skel_path)
            time.sleep(1)
