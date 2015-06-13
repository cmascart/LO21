#include "window_addtacheunaire.h"
#include "ui_window_addtacheunaire.h"
#include "QDateTime"
#include "QMessageBox"
#include "QStringListModel"
#include "model.h"

window_addtacheunaire::window_addtacheunaire(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::window_addtacheunaire)
{
    ui->setupUi(this);
    ui->rb_unaire->setChecked(true);
    ui->gb_unaire->setEnabled(true);
    ui->cb_prereq->setChecked(false);
    ui->gb_prereq->setEnabled(false);


    std::list<Tache*> taches = Model::getTaches();
    QStringList listeTaches;

    std::list<Tache*>::iterator it = taches.begin();
    for (unsigned int i = 0 ; i < taches.size() ; i++)
    {
        listeTaches << (*it)->getTitre();
        it++;
    }

    QStringListModel * modele = new QStringListModel(listeTaches);
    ui->list_prereq->setModel(modele);
    ui->list_prereq->setSelectionMode(QAbstractItemView::ExtendedSelection);

    QDateTime today = QDateTime::currentDateTime();
    ui->dt_dispo->setMinimumDateTime(today);
    ui->dt_ech->setMinimumDateTime(today);
    ui->dt_dispo->setDateTime(today.addDays(1));
    ui->dt_ech->setDateTime(today.addDays(1));

    connect(ui->rb_preemp, SIGNAL(clicked()), this, SLOT(click_tacheunaire()));
    connect(ui->rb_unaire, SIGNAL(clicked()), this, SLOT(click_tachepre()));
    connect(ui->cb_prereq, SIGNAL(clicked()), this, SLOT(click_prerequis()));
    connect(ui->b_ok, SIGNAL(clicked()), this, SLOT(click_bok()));
}

window_addtacheunaire::~window_addtacheunaire()
{
    delete ui;
}

void window_addtacheunaire::click_tacheunaire()
{
    ui->gb_unaire->setEnabled(false);
}

void window_addtacheunaire::click_tachepre()
{
    ui->gb_unaire->setEnabled(true);
}

void window_addtacheunaire::click_prerequis()
{
    if (ui->cb_prereq->isChecked())
        ui->gb_prereq->setEnabled(true);
    else
        ui->gb_prereq->setEnabled(false);
}

void window_addtacheunaire::click_bok()
{
    if (ui->t_titre->text().isEmpty())
    {
        QMessageBox::critical(this, "Erreur", "Veuillez entrer un titre");
        return;
    }
    if (ui->dt_dispo->dateTime()>ui->dt_ech->dateTime())
    {
        QMessageBox::critical(this, "Erreur", "La date de disponibilité est supérieure à la date d'échéance");
        return;
    }
    TacheUnitaire* newTache;

    if (ui->rb_preemp->isChecked())
    {
         newTache = new TacheUnitaire (ui->t_titre->text(), ui->dt_dispo->dateTime(), ui->dt_ech->dateTime(), ui->t_duree->time(), true);
         Model::ajouterTache(newTache);
    }
    else
    {
        newTache = new TacheUnitaire (ui->t_titre->text(), ui->dt_dispo->dateTime(), ui->dt_ech->dateTime(), ui->t_duree->time(), false);
        Model::ajouterTache(newTache);
    }

    if (ui->cb_prereq->isChecked()&&newTache!=NULL)
    {
        QItemSelectionModel *selection = ui->list_prereq->selectionModel();
        QModelIndexList listeSelections = selection->selectedIndexes();

        for (int i = 0 ; i < listeSelections.size() ; i++)
        {
            QVariant titreTache = modele->data(listeSelections[i], Qt::DisplayRole);
            TacheUnitaire * selectedTache = TacheManager::getTache(titreTache.toString());
            QMessageBox::critical(this, "Erreur", titreTache.toString());
            newTache->ajouterPrerequis(selectedTache);
        }

    }

    this->close();
}
