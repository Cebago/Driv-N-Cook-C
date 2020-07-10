#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "prototype.h"

GtkBuilder * builder;
GError *err = NULL;
GtkWidget * firstaname_e;
GtkWidget * lastname_e;
GtkWidget * mail_e;
GtkWidget * phone_e;
GtkWidget * address_e;
GtkWidget * city_e;
GtkWidget * postalCode_e;
GtkWidget * licenseID_e;
GtkWidget * account_e;

GtkWidget * labelStatus;
GtkWidget * buttonResetAll;

conf_file_t * globalConfRead;

int isAlreadySaved = 0;

gchar * getToUpper(gchar * s){
	for (int i = 0; s[i]!='\0'; i++) {
      if(s[i] >= 'a' && s[i] <= 'z') {
         s[i] = s[i] -32;
      }
   }
}

int isInChains(const gchar * string, char carac){
	for(int i = 0; i<strlen(string);i++){
		if (string[i] == carac) return 1;
	}
	return 0;
}

int isNumber(const gchar * string){
	for(int i = 0; i<strlen(string);i++){
		if (!isdigit(string[i])) return 0;
	}
	return 1;
}

int haveNumber(const gchar * string){
	for(int i = 0; i<strlen(string);i++){
		if (isdigit(string[i])) return 1;
	}
	return 0;
}


void destroyWindow(){
	gtk_main_quit();
	
}

void resetAll(GtkWidget * resetAll){
	gtk_entry_set_text(GTK_ENTRY(phone_e), "");
	gtk_entry_set_text(GTK_ENTRY(firstaname_e), "");
	gtk_entry_set_text(GTK_ENTRY(lastname_e), "");
	gtk_entry_set_text(GTK_ENTRY(mail_e), "");
	gtk_entry_set_text(GTK_ENTRY(phone_e), "");
	gtk_entry_set_text(GTK_ENTRY(address_e), "");
	gtk_entry_set_text(GTK_ENTRY(city_e), "");
	gtk_entry_set_text(GTK_ENTRY(postalCode_e), "");
	gtk_entry_set_text(GTK_ENTRY(licenseID_e), "");
	gtk_entry_set_text(GTK_ENTRY(account_e), "");
	isAlreadySaved = 0;
	gtk_widget_set_visible(buttonResetAll,0);
	gtk_label_set_text(GTK_LABEL(labelStatus), (const gchar * )"");

}

void addUser(GtkWidget * button){

	if (isAlreadySaved){
		gtk_label_set_markup(GTK_LABEL(labelStatus), (const gchar * )"<span foreground=\"#00C400\" >L'utilisateur à déjà été enregitré</span>");
		return;
	} 


	entries_t entries;
	
	entries.firstname = gtk_entry_get_text(GTK_ENTRY(firstaname_e));
	entries.lastnameTemp = gtk_entry_get_text(GTK_ENTRY(lastname_e));
	entries.mail = gtk_entry_get_text(GTK_ENTRY(mail_e));
	entries.phone = gtk_entry_get_text(GTK_ENTRY(phone_e));
	entries.address = gtk_entry_get_text(GTK_ENTRY(address_e));
	entries.city = gtk_entry_get_text(GTK_ENTRY(city_e));
	entries.postalCode = gtk_entry_get_text(GTK_ENTRY(postalCode_e));
	entries.licenseID = gtk_entry_get_text(GTK_ENTRY(licenseID_e));
	entries.account = gtk_entry_get_text(GTK_ENTRY(account_e));





	char * labelValue = malloc(100);

	if( strlen(entries.firstname)== 0 ||
		strlen(entries.lastnameTemp)== 0 ||
		strlen(entries.mail)== 0 ||
		strlen(entries.phone)== 0 ||
		strlen(entries.address)== 0 ||
		strlen(entries.city)== 0 ||
		strlen(entries.postalCode)== 0 ||
		strlen(entries.licenseID)== 0 ||
		strlen(entries.account)== 0
		){
		gtk_label_set_markup(GTK_LABEL(labelStatus), (const gchar * )"<span foreground=\"#FF0000\" >Tous les champs n'ont pas étés remplis </span>");
		//gtk_label_set_markup(GTK_LABEL(labelStatus), (const gchar * )"<span foreground=\"#FF0000\" >Là, Tous les champs n'ont pas étés remplis </span>");
		return;
	} 
	if(strlen(entries.firstname)<2 || haveNumber(entries.firstname) ){
		gtk_label_set_markup(GTK_LABEL(labelStatus), (const gchar * )"<span foreground=\"#FF0000\" >Le prénom n'est pas valide</span>");
		return;
	} 
	if(strlen(entries.lastnameTemp)<2 || haveNumber(entries.lastnameTemp) ){
		gtk_label_set_markup(GTK_LABEL(labelStatus), (const gchar * )"<span foreground=\"#FF0000\" >Le nom n'est pas valide</span>");
		return;
	} 
	if(strlen(entries.mail)<2 || !isInChains(entries.mail, '@') ){
		gtk_label_set_markup(GTK_LABEL(labelStatus), (const gchar * )"<span foreground=\"#FF0000\" >Le mail n'est pas valide</span>");
		return;
	} 
	if(strlen(entries.phone) != 10 || !isNumber(entries.phone) ){
		gtk_label_set_markup(GTK_LABEL(labelStatus), (const gchar * )"<span foreground=\"#FF0000\" >Le téléphone n'est pas valie</span>");
		return;
	} 
	if(strlen(entries.address)<2){
		gtk_label_set_markup(GTK_LABEL(labelStatus), (const gchar * )"<span foreground=\"#FF0000\" >L'adresse n'est pas valide</span>");
		return;
	} 
	if(strlen(entries.city)<2 || haveNumber(entries.city)){
		gtk_label_set_markup(GTK_LABEL(labelStatus), (const gchar * )"<span foreground=\"#FF0000\" >La ville n'est pas valide</span>");
		return;
	} 
	if(strlen(entries.postalCode) != 5 || !isNumber(entries.postalCode)){
		gtk_label_set_markup(GTK_LABEL(labelStatus), (const gchar * )"<span foreground=\"#FF0000\" >Le code postal n'est pas assez long</span>");
		return;
	} 
	if(strlen(entries.licenseID) != 12 || !isNumber(entries.licenseID)){
		gtk_label_set_markup(GTK_LABEL(labelStatus), (const gchar * )"<span foreground=\"#FF0000\" >Le numéro de permis n'est pas valide</span>");
		return;
	} 	
	char *tmp;
 	long account = strtol(entries.account, &tmp, 10);
	if(!isNumber(entries.account) || account>50000){
		gtk_label_set_markup(GTK_LABEL(labelStatus), (const gchar * )"<span foreground=\"#FF0000\" >L'accompte n'est pas valide</span>");
		return;
	} 



	gchar * lastname = (gchar *)entries.lastnameTemp; 
	getToUpper(lastname);

	FILE * file = fopen("data.txt","w+b");
	if(!file){
		printf("Cannot open file\n");
		return;
	}
	fprintf(file, "%s,%s,%s,%s,%s,%s,%s,%s,%s",entries.firstname, lastname, entries.mail, entries.phone, entries.address, entries.postalCode, entries.city, entries.licenseID, entries.account);
	fflush(file);
	printf("Donnée glade écrites\n");
	const char* qrcodePath = "qrcode.png";
	qrGenerate(file, qrcodePath);
	printf("QR code généré\n");
	upload(qrcodePath);
	printf("Envoie serveur en cours...\n");
	if(checkUser((char *)entries.mail)){
		gtk_label_set_markup(GTK_LABEL(labelStatus), (const gchar * )"<span foreground=\"#00C400\" >L'utilisateur a bien été enregitré en base</span>");
		isAlreadySaved = 1;
		gtk_widget_set_visible(buttonResetAll,1);
	}else{
		gtk_label_set_markup(GTK_LABEL(labelStatus), (const gchar * )"<span foreground=\"#FF0000\" >L'utilisateur n'a pas pu être inseré en base</span>");

	}





}


int main(){

	globalConfRead = getConf();

	GtkWidget *w_window;
	GtkWidget *button;
	gtk_init(NULL, NULL);
	builder = gtk_builder_new();
	gtk_builder_add_from_file (builder,"glade_pa.glade", NULL);
	w_window = GTK_WIDGET(gtk_builder_get_object(builder,"w_window"));

	firstaname_e =  GTK_WIDGET(gtk_builder_get_object(builder, "firstname"));
	lastname_e =  GTK_WIDGET(gtk_builder_get_object(builder, "lastname"));
	mail_e =  GTK_WIDGET(gtk_builder_get_object(builder, "mail"));
	phone_e =  GTK_WIDGET(gtk_builder_get_object(builder, "phone"));
	address_e =  GTK_WIDGET(gtk_builder_get_object(builder, "address"));
	city_e =  GTK_WIDGET(gtk_builder_get_object(builder, "city"));
	postalCode_e =  GTK_WIDGET(gtk_builder_get_object(builder, "postalCode"));
	licenseID_e =  GTK_WIDGET(gtk_builder_get_object(builder, "licenseID"));
	account_e =  GTK_WIDGET(gtk_builder_get_object(builder, "account"));
	labelStatus =  GTK_WIDGET(gtk_builder_get_object(builder, "labelStatus"));

	buttonResetAll = GTK_WIDGET(gtk_builder_get_object(builder, "resetAll"));

	gtk_builder_connect_signals(builder,NULL);
	g_object_unref(builder);
	gtk_widget_show(w_window);
	gtk_main();
	cleanConfFile(globalConfRead);
	return 0;


}
