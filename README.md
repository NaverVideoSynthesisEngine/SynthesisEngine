# SynthesisEngine

A Engine making dataset for 3D pose estimation.<br/> Using Unreal Engine 4 and MarvelousDesinger.

# Project RoadMap
Last Update : Oct 24th 2019.

## **Before Synthesis**
| Category                                | Object                                                          | Current Progress |
|-----------------------------------------|-----------------------------------------------------------------|------------------|
| Human Animations                        | Automate download human animations from Mixamo                  | 90%              |
| Clothes Animations                      | Automate generating clothes animations using Marvelous Designer | 5%               |
| Purchase Assets (Background, Materials) | Purchase materials, background assets.                          | 70%              |

## **Synthesis**

| Category            | Object                                                             | Current Progress |
|---------------------|--------------------------------------------------------------------|------------------|
| Animation & Clothes | Proceed animation by one frame or change clothes during synthesis. | 60%              |
| Camera              | Change camera position during synthesis.                           | 90%              |
| Materials           | Change color or texture of clothes.                                | 30%              |
| Lights              | Change light settings. (Sun, Light bulbs)                          | 50%              |
| Occlusion           | Multiple humans in an image. Annotate occlusion information.       | 10%              |

## **Remaining Tasks**

### Before Synthesis

#### Human Animations(90%) : 
> 1. Mixamo Download is working well. Download actual data using it.

#### Clothes Animations(5%) :  
> 1. Check Test Build from Marvelous Designer (arrived Oct 23th.)

#### Purchase Assets (Background, Materials): (70%) 
> 1. Purchase Fabric Materials https://youtu.be/3onVrSbxbgY 
> 2. Two more background asset(levels)

### Synthesis

#### Animation & Clothes(60%) : 
> 1. Synchronize Human Animation and Clothes Animation
> 2. Decide to use alembic or obj sequence.
> 3. Add feature for multiple combination of clothes (for now, we can handle combination of clothes which means human can wear multiple clothes at same time. but we can handle only one combination for a human.)

#### Camera(90%) : 
> 1. Make sure that there is nothing between camera and human. (There can be wall, ceiling or blocking things. (especially in a small house or small background))

#### Materials: (70%)
> 1. Apply Fabric Materials https://youtu.be/3onVrSbxbgY 

#### Occlusion: (10%)
> 1. Add one or multiple humans on 'PhotoRoom'
> 2. Collision Check feature


