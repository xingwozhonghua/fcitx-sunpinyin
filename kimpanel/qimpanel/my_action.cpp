/*
 * Copyright (C) 2013 National University of Defense Technology(NUDT) & Kylin Ltd.
 *
 * Authors:
 *  lenky gao    lenky0401@gmail.com/gaoqunkai@ubuntukylin.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "my_action.h"


MyAction::MyAction(const QString &text, QObject *parent) : QAction(text, parent)
{

}

MyAction::MyAction(const QIcon &icon, const QString &text, QObject *parent)
    : QAction(text, parent)
{
    this->setIcon(QIcon(icon.pixmap(QSize(16,16))));
}

MyAction::~MyAction()
{

}

void MyAction::setProp(const KimpanelProperty &prop)
{
    this->mProp = prop;
}

const KimpanelProperty& MyAction::getProp()
{
    return this->mProp;
}

void MyAction::setSkinPath(const QString skinPath)
{
    this->mSkinPath = skinPath;
}

const QString MyAction::getSkinPath()
{
    return this->mSkinPath;
}

void MyAction::setSkinClass(const SkinClass skinClass)
{
    this->mSkinClass = skinClass;
}

const SkinClass MyAction::getSkinClass()
{
    return this->mSkinClass;
}

void MyAction::setMyActionType(const MyActionType myActionType)
{
    this->mMyActionType = myActionType;
}

const MyActionType MyAction::getMyActionType()
{
    return this->mMyActionType;
}

