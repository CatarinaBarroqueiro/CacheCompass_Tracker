
import { Routes } from '@angular/router';
import { LogInComponent } from './components/log-in/log-in.component';
import { RegisterComponent } from './components/register/register.component';
import { HistoryComponent } from './history/history.component';

export const routes: Routes = [
    { path: '', pathMatch: 'full', redirectTo: 'login' },
    { path: 'login', component: LogInComponent },
    { path: 'register', component: RegisterComponent },
    { path: 'history', component: HistoryComponent }


];
