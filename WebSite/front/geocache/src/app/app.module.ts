import { NgModule, CUSTOM_ELEMENTS_SCHEMA } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { AppComponent } from './app.component';
import { HttpClientModule } from '@angular/common/http';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { platformBrowserDynamic } from '@angular/platform-browser-dynamic';
import { MaterialModule } from './material/material.module';
import { FlexLayoutModule } from '@angular/flex-layout';
import { MatCardModule } from '@angular/material/card';
import { MatFormFieldModule } from '@angular/material/form-field'; 
import { MatSelectModule } from '@angular/material/select';
import { MatOptionModule } from '@angular/material/core';
import { LogInComponent } from './components/log-in/log-in.component';
import { RegisterComponent } from './components/register/register.component';
import { NavbarComponent } from './layout/navbar/navbar.component';
import { MatSlideToggleModule } from '@angular/material/slide-toggle';
import { FooterComponent } from './layout/footer/footer.component';
import { HistoryComponent } from './history/history.component';
import { DialogComponent } from './dialog/dialog.component';


@NgModule({
  declarations: [
    AppComponent,
    LogInComponent,
    RegisterComponent,
    HistoryComponent,
    NavbarComponent,
    FooterComponent,
    DialogComponent
   ],
  imports: [
    MatSlideToggleModule,
    FlexLayoutModule,
    BrowserModule,
    HttpClientModule,
    BrowserAnimationsModule,
    MaterialModule,
    FormsModule,
    ReactiveFormsModule,
    MatCardModule,
    MatSelectModule,
    MatOptionModule,
    MatFormFieldModule
  ],
  schemas: [CUSTOM_ELEMENTS_SCHEMA],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }

platformBrowserDynamic().bootstrapModule(AppModule);