import unreal


def import_alembic(abc_path, asset_path):
    # Create an import task.
    import_task = unreal.AssetImportTask()

    # Set base properties on the task.
    import_task.filename = abc_path
    import_task.destination_path = asset_path
    import_task.automated = True  # Suppress UI.
    import_task.options = _get_alembic_import_options(frame_start=0, frame_end=3)

    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    asset_tools.import_asset_tasks([import_task])


def _get_alembic_import_options(frame_start=0, frame_end=3):
    options = unreal.AbcImportSettings()
    options.import_type = unreal.AlembicImportType.SKELETAL
    options.conversion_settings = unreal.AbcConversionSettings(preset=unreal.AbcConversionPreset.MAX)

    options.sampling_settings.frame_start = frame_start
    options.sampling_settings.frame_end = frame_end


def run():
    abc_path = "D:/workspace/temp/sad.abc"
    asset_path = '/Game/Alembic'

    import_alembic(abc_path, asset_path)
