/*
 * translations.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

#include "StdInc.h"
#include "translations.h"
#include "ui_translations.h"
#include "../../lib/Languages.h"
#include "../../lib/CGeneralTextHandler.h"
#include "../../lib/VCMI_Lib.h"

void Translations::cleanupRemovedItems(CMap & map)
{
	std::set<std::string> existingObjects;
	for(auto object : map.objects)
		existingObjects.insert(object->instanceName);
	
	for(auto & translations : map.translations.Struct())
	{
		auto updateTranslations = JsonNode(JsonNode::JsonType::DATA_STRUCT);
		for(auto & s : translations.second.Struct())
		{
			for(auto part : QString::fromStdString(s.first).split('.'))
			{
				if(existingObjects.count(part.toStdString()))
				{
					updateTranslations.Struct()[s.first] = s.second;
					break;
				}
			}
		}
		translations.second = updateTranslations;
	}
}

void Translations::cleanupRemovedItems(CMap & map, const std::string & pattern)
{
	for(auto & translations : map.translations.Struct())
	{
		auto updateTranslations = JsonNode(JsonNode::JsonType::DATA_STRUCT);
		for(auto & s : translations.second.Struct())
		{
			if(s.first.find(pattern) == std::string::npos)
				updateTranslations.Struct()[s.first] = s.second;
		}
		translations.second = updateTranslations;
	}
}

Translations::Translations(CMapHeader & mh, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Translations),
	mapHeader(mh)
{
	ui->setupUi(this);
	
	//fill languages list
	for(auto & language : Languages::getLanguageList())
	{
		ui->languageSelect->blockSignals(true);
		ui->languageSelect->addItem(QString("%1 (%2)").arg(QString::fromStdString(language.nameEnglish), QString::fromStdString(language.nameNative)));
		ui->languageSelect->setItemData(ui->languageSelect->count() - 1, QVariant(QString::fromStdString(language.identifier)));
		ui->languageSelect->blockSignals(false);
		if(language.identifier == VLC->generaltexth->getPreferredLanguage())
			ui->languageSelect->setCurrentIndex(ui->languageSelect->count() - 1);
	}
}

Translations::~Translations()
{
	delete ui;
}

void Translations::fillTranslationsTable(const std::string & language)
{
	Translations::cleanupRemovedItems(dynamic_cast<CMap&>(mapHeader));
	auto & translation = mapHeader.translations[language];
	ui->translationsTable->blockSignals(true);
	ui->translationsTable->setRowCount(0);
	ui->translationsTable->setRowCount(translation.Struct().size());
	int i = 0;
	for(auto & s : translation.Struct())
	{
		auto * wId = new QTableWidgetItem(QString::fromStdString(s.first));
		auto * wText = new QTableWidgetItem(QString::fromStdString(s.second.String()));
		wId->setFlags(wId->flags() & ~Qt::ItemIsEditable);
		wText->setFlags(wId->flags() | Qt::ItemIsEditable);
		ui->translationsTable->setItem(i, 0, wId);
		ui->translationsTable->setItem(i++, 1, wText);
	}
	ui->translationsTable->resizeColumnToContents(0);
	ui->translationsTable->blockSignals(false);
}

void Translations::on_languageSelect_currentIndexChanged(int index)
{
	auto language = ui->languageSelect->currentData().toString().toStdString();
	bool hasLanguage = !mapHeader.translations[language].isNull();
	ui->supportedCheck->blockSignals(true);
	ui->supportedCheck->setChecked(hasLanguage);
	ui->supportedCheck->blockSignals(false);
	ui->translationsTable->setEnabled(hasLanguage);
	if(hasLanguage)
		fillTranslationsTable(language);
	else
		ui->translationsTable->setRowCount(0);
}


void Translations::on_supportedCheck_toggled(bool checked)
{
	auto language = ui->languageSelect->currentData().toString().toStdString();
	auto & translation = mapHeader.translations[language];
	bool hasRecord = !translation.Struct().empty();
	
	if(checked)
	{
		//copy from default language
		translation = mapHeader.translations[VLC->generaltexth->getPreferredLanguage()];
		
		fillTranslationsTable(language);
		ui->translationsTable->setEnabled(true);
	}
	else
	{
		bool canRemove = language != VLC->generaltexth->getPreferredLanguage();
		if(!canRemove)
		{
			QMessageBox::information(this, tr("Remove translation"), tr("Default language cannot be removed"));
		}
		else if(hasRecord)
		{
			auto sure = QMessageBox::question(this, tr("Remove translation"), tr("This language has text records which will be removed. Continue?"));
			canRemove = sure != QMessageBox::No;
		}
		
		if(!canRemove)
		{
			ui->supportedCheck->blockSignals(true);
			ui->supportedCheck->setChecked(true);
			ui->supportedCheck->blockSignals(false);
			return;
		}
		ui->translationsTable->blockSignals(true);
		ui->translationsTable->setRowCount(0);
		translation = JsonNode(JsonNode::JsonType::DATA_NULL);
		ui->translationsTable->blockSignals(false);
		ui->translationsTable->setEnabled(false);
	}
}


void Translations::on_translationsTable_itemChanged(QTableWidgetItem * item)
{
	assert(item->column() == 1);
	
	auto language = ui->languageSelect->currentData().toString().toStdString();
	auto & translation = mapHeader.translations[language];
	
	assert(!translation.isNull());
	
	auto textId = ui->translationsTable->item(item->row(), 0)->text().toStdString();
	assert(!textId.empty());
	if(textId.empty())
		return;
	
	translation[textId].String() = item->text().toStdString();
}

