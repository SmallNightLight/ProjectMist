#pragma once

#include "Rect.h"
#include "../Base/Settings.h"

#include <iostream>
#include <memory>
#include <vector>
#include <array>
#include <list>

class QuadTree
{
public:
    explicit QuadTree(const Rect& pArea, const int32_t pDepth = 0) : depth(pDepth)
    {
        Resize(pArea);
    }

    void Resize(const Rect& pArea)
    {
        Clear();
        area = pArea;
        glm::vec2 childArea = area.Size / 2.0f;

        childrenRects =
        {
            Rect(area.Position, childArea),
            Rect({area.Position.x + childArea.x, area.Position.y}, childArea),
            Rect({area.Position.x, area.Position.y + childArea.y}, childArea),
            Rect(area.Position + childArea, childArea)
        };
    }

    void Clear()
    {
        items.clear();
        for(int i = 0; i < 4; i++)
        {
            if (childrenTrees[i])
                childrenTrees[i]->Clear();

            childrenTrees[i].reset();
        }
    }

    [[nodiscard]] int32_t ItemCount() const
    {
        int32_t count = items.size();
        for(int i = 0; i < 4; i++)
            if (childrenTrees[i])
                count += childrenTrees[i]->ItemCount();

        return count;
    }

    void Insert(const Entity& item, const Rect& itemArea)
    {
        for (int i= 0; i < 4; i++)
        {
            if (childrenRects[i].Contains(itemArea))
            {
                if (depth + 1 < QUADTREE_MAX_DEPTH)
                {
                    if (!childrenTrees[i])
                    {
                        childrenTrees[i] = std::make_shared<QuadTree>(childrenRects[i], depth + 1);
                    }

                    childrenTrees[i]->Insert(item, itemArea);
                    return;
                }
            }
        }

        items.push_back({ itemArea, item });
    }

    std::list<Entity> Search(const Rect& pArea) const
    {
        std::list<Entity> listItems;
        Search(pArea, listItems);
        //std::cout << listItems.size() << std::endl;
        return listItems;
    }

    void Search(const Rect& pArea, std::list<Entity>& listItems) const
    {
        for(const auto& p : items)
        {
            if(pArea.Overlaps(p.first))
                listItems.push_back(p.second);
        }

        for(int i = 0; i < 4; i++)
        {
            if (childrenTrees[i])
            {
                    if (area.Contains(childrenRects[i]))
                        childrenTrees[i]->Items(listItems);
                    else if (childrenRects[i].Overlaps(area))
                        childrenTrees[i]->Search(area, listItems);
            }
        }
    }

    void Items(std::list<Entity>& listItems) const
    {
        for(const auto& item : items)
            listItems.push_back(item.second);

        for(int i = 0; i < 4; i++)
        {
            if (childrenTrees[i])
                childrenTrees[i]->Items(listItems);
        }
    }

    std::list<Rect> GetObjectBounds()
    {
        std::list<Rect> listItems;
        GetObjectBounds(listItems);
        return listItems;
    }

    void GetObjectBounds(std::list<Rect>& listItems) const
    {
        for(const auto& item : items)
            listItems.push_back(item.first);

        for(int i = 0; i < 4; i++)
        {
            if (childrenTrees[i])
                childrenTrees[i]->GetObjectBounds(listItems);
        }
    }

    std::list<Rect> GetQuadBounds()
    {
        std::list<Rect> listItems;
        GetQuadBounds(listItems);
        return listItems;
    }

    void GetQuadBounds(std::list<Rect>& listItems) const
    {
        listItems.push_back(area);

        for(int i = 0; i < 4; i++)
        {
            if (childrenTrees[i])
                childrenTrees[i]->GetQuadBounds(listItems);
        }
    }

    const Rect& Area()
    {
        return area;
    }

private:
    int32_t depth = 0;
    Rect area;
    std::array<Rect, 4> childrenRects;
    std::array<std::shared_ptr<QuadTree>, 4> childrenTrees { }; //ToDO: remove pointer and store with indexing
    std::vector<std::pair<Rect, Entity>> items;
};