/*
    custom icon engine Qt plugin
    Copyright (C) 2017-2023  Nick Korotysh <nick.korotysh@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <QIconEngine>
#include <QScopedPointer>

class QSvgRenderer;

class PaletteIconEngine : public QIconEngine
{
public:
  PaletteIconEngine();
  PaletteIconEngine(const PaletteIconEngine& other);
  ~PaletteIconEngine();

  void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;
  QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;

  void addFile(const QString& fileName, const QSize& size, QIcon::Mode mode, QIcon::State state) override;

  QString key() const override;
  QIconEngine* clone() const override;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  QList<QSize> availableSizes(QIcon::Mode mode, QIcon::State state) const override;
#else
  QList<QSize> availableSizes(QIcon::Mode mode, QIcon::State state) override;
#endif

  void virtual_hook(int id, void* data) override;

private:
  QScopedPointer<QSvgRenderer> renderer_;
  QString src_file_;
};
